#include "synthesis.h"

#include "verification/MdpModelChecker.h"

#include "storm/modelchecker/CheckTask.h"
#include "storm/modelchecker/results/CheckResult.h"
#include "storm/environment/Environment.h"
#include "storm/api/verification.h"
#include "storm/modelchecker/hints/ExplicitModelCheckerHint.h"

#include "storm/storage/SparseMatrix.h"

#include "storm/utility/initialize.h"

#include "storm/environment/solver/NativeSolverEnvironment.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"

namespace synthesis {

    template<typename ValueType>
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
    }

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
    
}


// Define python bindings
void define_helpers(py::module& m) {

    m.def("set_loglevel_off", []() { storm::utility::setLogLevel(l3pp::LogLevel::OFF); }, "set loglevel for storm to off");

    m.def("multiply_with_vector", [] (storm::storage::SparseMatrix<double> matrix,std::vector<double> vector) {
        std::vector<double> result(matrix.getRowCount());
        matrix.multiplyWithVector(vector, result);
        return result;
    }, py::arg("matrix"), py::arg("vector"));

    m.def("model_check_with_hint", &synthesis::modelCheckWithHint<double>);
    m.def("transform_until_to_eventually", &synthesis::transformUntilToEventually<double>, py::arg("formula"));
    
    m.def("construct_selection", [] ( storm::storage::BitVector default_actions, std::vector<uint_fast64_t> selected_actions) {
        auto bv = storm::storage::BitVector(default_actions);
        for(auto action: selected_actions)
            bv.set(action);
        return bv;
    }, py::arg("default_actions"), py::arg("selected_actions"));

    m.def("set_precision_native", [](storm::NativeSolverEnvironment& nsenv, double value) {
        nsenv.setPrecision(storm::utility::convertNumber<storm::RationalNumber>(value));
    });
    m.def("set_precision_minmax", [](storm::MinMaxSolverEnvironment& nsenv, double value) {
        nsenv.setPrecision(storm::utility::convertNumber<storm::RationalNumber>(value));
    });

    m.def("verify_mdp", &synthesis::verifyMdp<double>);
}

