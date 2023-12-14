#include "synthesis.h"

#include "src/synthesis/verification/MdpModelChecker.h"
#include "src/synthesis/quotient/Family.h"
#include "src/synthesis/quotient/Coloring.h"

#include <storm/modelchecker/CheckTask.h>
#include <storm/modelchecker/results/CheckResult.h>
#include <storm/environment/Environment.h>
#include <storm/api/verification.h>
#include <storm/modelchecker/hints/ExplicitModelCheckerHint.h>

#include <storm/storage/SparseMatrix.h>
#include <storm/utility/initialize.h>
#include <storm/environment/solver/NativeSolverEnvironment.h>
#include <storm/environment/solver/MinMaxSolverEnvironment.h>

#include <queue>

namespace synthesis {

/*template<typename ValueType>
std::shared_ptr<storm::modelchecker::CheckResult> modelCheckWithHint(
    std::shared_ptr<storm::models::sparse::Model<ValueType>> model,
    storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> & task,
    storm::Environment const& env,
    std::vector<ValueType> hint_values
) {
    storm::modelchecker::ExplicitModelCheckerHint<ValueType> hint;
    hint.setComputeOnlyMaybeStates(false);
    hint.setNoEndComponentsInMaybeStates(false);
    hint.setResultHint(boost::make_optional(hint_values));
    task.setHint(std::make_shared<storm::modelchecker::ExplicitModelCheckerHint<ValueType>>(hint));
    return storm::api::verifyWithSparseEngine<ValueType>(env, model, task);
}*/

template<typename ValueType>
std::shared_ptr<storm::logic::Formula> transformUntilToEventually(
    storm::logic::Formula const& formula
) {
    auto const& of = formula.asOperatorFormula();
    bool is_reward = of.isRewardOperatorFormula();

    auto ef = std::make_shared<storm::logic::EventuallyFormula>(
        of.getSubformula().asUntilFormula().getRightSubformula().asSharedPointer(),
        !is_reward ? storm::logic::FormulaContext::Probability : storm::logic::FormulaContext::Reward);

    std::shared_ptr<storm::logic::Formula> modified_formula;
    if(!is_reward) {
        modified_formula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(ef, of.getOperatorInformation());
    } else {
        modified_formula = std::make_shared<storm::logic::RewardOperatorFormula>(ef, of.asRewardOperatorFormula().getRewardModelName(), of.getOperatorInformation());
    }

    return modified_formula;
}

template<typename ValueType>
std::vector<std::vector<uint64_t>> computeChoiceDestinations(storm::models::sparse::Mdp<ValueType> const& mdp) {
    uint64_t num_choices = mdp.getNumberOfChoices();
    std::vector<std::vector<uint64_t>> choice_destinations(num_choices);
    for(uint64_t choice = 0; choice < num_choices; ++choice) {
        for(auto const& entry: mdp.getTransitionMatrix().getRow(choice)) {
            choice_destinations[choice].push_back(entry.getColumn());
        }
    }
    return choice_destinations;
}

template<typename ValueType>
std::vector<uint64_t> schedulerToStateToGlobalChoice(
    storm::storage::Scheduler<ValueType> const& scheduler, storm::models::sparse::Mdp<ValueType> const& sub_mdp,
    std::vector<uint64_t> choice_to_global_choice
) {
    uint64_t num_states = sub_mdp.getNumberOfStates();
    std::vector<uint64_t> state_to_choice(num_states);
    auto const& nci = sub_mdp.getNondeterministicChoiceIndices();
    for(uint64_t state=0; state<num_states; ++state) {
        uint64_t choice = nci[state] + scheduler.getChoice(state).getDeterministicChoice();
        uint64_t choice_global = choice_to_global_choice[choice];
        state_to_choice[state] = choice_global;
    }
    return state_to_choice;
}

/*template<typename ValueType>
std::vector<uint64_t> schedulerToGlobalScheduler(
    storm::models::sparse::Mdp<ValueType> const& sub_mdp,
    std::vector<uint64_t> choice_to_global_choice,
    std::vector<uint64_t> state_to_global_state,
    storm::storage::Scheduler<ValueType> scheduler,
    uint64_t invalid_choice
) {
    std::vector<uint64_t> state_to_choice(sub_mdp.getNumberOfStates(),invalid_choice);
    auto const& nci = sub_mdp.getNondeterministicChoiceIndices();
    for(uint64_t state=0; state<sub_mdp.getNumberOfStates(); ++state) {
        uint64_t choice = nci[state] + scheduler.getChoice(state).getDeterministicChoice();
        uint64_t choice_global = choice_to_global_choice[choice];
        uint64_t state_global = state_to_global_state[state];
        state_to_choice[state_global] = choice_global;
    }
    return state_to_choice;
}
*/

std::map<uint64_t,double> computeInconsistentHoleVariance(
    Family const& family,
    std::vector<uint64_t> const& row_groups, std::vector<uint64_t> const& choice_to_global_choice, std::vector<double> const& choice_to_value,
    Coloring const& coloring, std::map<uint64_t,std::vector<uint64_t>> const& hole_to_inconsistent_options,
    std::vector<double> const& state_to_expected_visits
) {

    auto num_holes = family.numHoles();
    std::vector<BitVector> hole_to_inconsistent_options_mask(num_holes);
    for(uint64_t hole=0; hole<num_holes; ++hole) {
        hole_to_inconsistent_options_mask[hole] = BitVector(family.holeNumOptionsTotal(hole));
    }
    BitVector inconsistent_holes(num_holes);
    for(auto const& [hole,options]: hole_to_inconsistent_options) {
        inconsistent_holes.set(hole);
        for(auto option: options) {
            hole_to_inconsistent_options_mask[hole].set(option);
        }
    }

    std::vector<double> hole_difference_avg(num_holes,0);
    std::vector<uint64_t> hole_states_affected(num_holes,0);
    auto const& choice_to_assignment = coloring.getChoiceToAssignment();
    
    std::vector<bool> hole_set(num_holes);
    std::vector<double> hole_min(num_holes);
    std::vector<double> hole_max(num_holes);
        
    auto num_states = row_groups.size()-1;
    for(uint64_t state=0; state<num_states; ++state) {

        for(uint64_t choice=row_groups[state]; choice<row_groups[state+1]; ++choice) {
            auto value = choice_to_value[choice];
            auto choice_global = choice_to_global_choice[choice];
            
            for(auto const& [hole,option]: choice_to_assignment[choice_global]) {
                if(not  hole_to_inconsistent_options_mask[hole][option]) {
                    continue;
                }

                if(not hole_set[hole]) {
                    hole_min[hole] = value;
                    hole_max[hole] = value;
                    hole_set[hole] = true;
                } else {
                    if(value < hole_min[hole]) {
                        hole_min[hole] = value;
                    }
                    if(value > hole_max[hole]) {
                        hole_max[hole] = value;
                    }
                }
            }
        }

        for(auto hole: inconsistent_holes) {
            if(not hole_set[hole]) {
                continue;
            }
            double difference = (hole_max[hole]-hole_min[hole])*state_to_expected_visits[state];
            hole_states_affected[hole] += 1;
            hole_difference_avg[hole] += (difference-hole_difference_avg[hole]) / hole_states_affected[hole];
        }
        std::fill(hole_set.begin(), hole_set.end(), false);
    }

    std::map<uint64_t,double> inconsistent_hole_variance;
    for(auto hole: inconsistent_holes) {
        inconsistent_hole_variance[hole] = hole_difference_avg[hole];
    }

    return inconsistent_hole_variance;
}


/*storm::storage::BitVector keepReachableChoices(
    storm::storage::BitVector enabled_choices, uint64_t initial_state,
    std::vector<uint64_t> const& row_groups, std::vector<std::vector<uint64_t>> const& choice_destinations
) {

    uint64_t num_states = row_groups.size()-1;
    uint64_t num_choices = enabled_choices.size();

    storm::storage::BitVector reachable_choices(num_choices,false);
    storm::storage::BitVector state_visited(num_states,false);

    std::queue<uint64_t> state_queue;
    state_visited.set(initial_state,true);
    state_queue.push(initial_state);
    while(not state_queue.empty()) {
        auto state = state_queue.front();
        state_queue.pop();
        for(uint64_t choice = row_groups[state]; choice < row_groups[state+1]; ++choice) {
            if(not enabled_choices[choice]) {
                continue;
            }
            reachable_choices.set(choice,true);
            for(auto dst: choice_destinations[choice]) {
                if(not state_visited[dst]) {
                    state_visited.set(dst,true);
                    state_queue.push(dst);
                }
            }
        }
    }
    return reachable_choices;
}*/


// RA: even I don't understand why this needs to be optimized, but it does
storm::storage::BitVector policyToChoicesForFamily(
    std::vector<uint64_t> const& policy_choices,
    storm::storage::BitVector const& family_choices
) {
    storm::storage::BitVector choices(family_choices.size(),false);
    for(auto choice : policy_choices) {
        choices.set(choice,true);
    }
    return choices & family_choices;
}

/*std::pair<std::vector<uint64_t>,storm::storage::BitVector> fixPolicyForFamily(
    std::vector<uint64_t> const& policy, uint64_t invalid_action,
    storm::storage::BitVector const& family_choices,
    uint64_t initial_state, uint64_t num_choices,
    std::vector<std::vector<uint64_t>> const& state_to_actions,
    std::vector<std::vector<std::vector<uint64_t>>> const& state_action_choices,
    std::vector<std::vector<uint64_t>> const& choice_destinations
) {

    uint64_t num_states = state_to_actions.size();

    std::vector<uint64_t> policy_fixed(num_states,invalid_action);
    storm::storage::BitVector choice_mask(num_choices,false);

    storm::storage::BitVector state_visited(num_states,false);
    state_visited.set(initial_state,true);

    std::queue<uint64_t> state_queue;
    state_queue.push(initial_state);
    while(not state_queue.empty()) {
        auto state = state_queue.front();
        state_queue.pop();
        // get action executed in the state
        auto action = policy[state];
        if(action == invalid_action) {
            action = state_to_actions[state][0];
        }
        policy_fixed[state] = action;
        // expand through the choices that correspond to this action
        for(auto choice: state_action_choices[state][action]) {
            if(not family_choices[choice]) {
                continue;
            }
            choice_mask.set(choice,true);
            for(auto dst: choice_destinations[choice]) {
                if(not state_visited[dst]) {
                    state_visited.set(dst,true);
                    state_queue.push(dst);
                }
            }   
        }
    }
    return std::make_pair(policy_fixed,choice_mask);
}*/

}

// Define python bindings
void define_helpers(py::module& m) {

    m.def("set_loglevel_off", []() { storm::utility::setLogLevel(l3pp::LogLevel::OFF); }, "set loglevel for storm to off");

    m.def("set_precision_native", [](storm::NativeSolverEnvironment& nsenv, double value) {
        nsenv.setPrecision(storm::utility::convertNumber<storm::RationalNumber>(value));
    });
    m.def("set_precision_minmax", [](storm::MinMaxSolverEnvironment& nsenv, double value) {
        nsenv.setPrecision(storm::utility::convertNumber<storm::RationalNumber>(value));
    });

    m.def("transform_until_to_eventually", &synthesis::transformUntilToEventually<double>, py::arg("formula"));

    m.def("multiply_with_vector", [] (storm::storage::SparseMatrix<double> matrix,std::vector<double> vector) {
        std::vector<double> result(matrix.getRowCount());
        matrix.multiplyWithVector(vector, result);
        return result;
    }, py::arg("matrix"), py::arg("vector"));

    m.def("verify_mdp", &synthesis::verifyMdp<double>);
    
    m.def("computeChoiceDestinations", &synthesis::computeChoiceDestinations<double>);

    m.def("schedulerToStateToGlobalChoice", &synthesis::schedulerToStateToGlobalChoice<double>);
    m.def("computeInconsistentHoleVariance", &synthesis::computeInconsistentHoleVariance);
    
    // m.def("keepReachableChoices", &synthesis::keepReachableChoices);
    m.def("policyToChoicesForFamily", &synthesis::policyToChoicesForFamily);

}

