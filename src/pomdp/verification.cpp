#include "verification.h"
#include <storm-pomdp/api/verification.h>

template<typename ValueType>
void define_verification(py::module& m, std::string const& vtSuffix) {
    typedef storm::pomdp::modelchecker::BeliefExplorationPomdpModelCheckerOptions<ValueType> Options;

    py::class_<typename storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>> bepmc(m, ("BeliefExplorationModelChecker" + vtSuffix).c_str());
    bepmc.def(py::init<std::shared_ptr<storm::models::sparse::Pomdp<ValueType>>, Options>(), py::arg("model"), py::arg("options"));
    bepmc.def("check", py::overload_cast<storm::logic::Formula const&, std::vector<std::vector<std::unordered_map<uint64_t, ValueType>>> const&>(&storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::check), py::call_guard<py::gil_scoped_release>());
    bepmc.def("pause_unfolding", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::pauseUnfolding);
    bepmc.def("continue_unfolding", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::continueUnfolding);
    bepmc.def("terminate_unfolding", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::terminateUnfolding);
    bepmc.def("is_result_ready", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::isResultReady);
    bepmc.def("is_exploring", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::isExploring);
    bepmc.def("get_interactive_result", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::getInteractiveResult);
    bepmc.def("get_status", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::getStatus);
    bepmc.def("get_interactive_belief_explorer", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::getInteractiveBeliefExplorer);
    bepmc.def("has_converged", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::hasConverged);

    py::class_<typename storm::builder::BeliefMdpExplorer<storm::models::sparse::Pomdp<ValueType>>> bmdpe(m, ("BeliefMdpExplorer" + vtSuffix).c_str());
    bmdpe.def("set_fsc_values", &storm::builder::BeliefMdpExplorer<storm::models::sparse::Pomdp<ValueType>>::setFMSchedValueList, py::arg("value_list"));

    py::class_<Options> bepmcoptions(m, ("BeliefExplorationModelCheckerOptions" + vtSuffix).c_str());
    bepmcoptions.def(py::init<bool, bool>(), py::arg("discretize"), py::arg("unfold"));
    bepmcoptions.def_readwrite("use_state_elimination_cutoff", &Options::useStateEliminationCutoff);
    bepmcoptions.def_readwrite("size_threshold_init", &Options::sizeThresholdInit);
    bepmcoptions.def_readwrite("use_clipping", &Options::useClipping);
    bepmcoptions.def_readwrite("exploration_time_limit", &Options::explorationTimeLimit);
    bepmcoptions.def_readwrite("clipping_grid_res", &Options::clippingGridRes);
    bepmcoptions.def_readwrite("gap_threshold_init", &Options::gapThresholdInit);
    bepmcoptions.def_readwrite("size_threshold_factor", &Options::sizeThresholdFactor);
    bepmcoptions.def_readwrite("refine_precision", &Options::refinePrecision);
    bepmcoptions.def_readwrite("refine_step_limit", &Options::refineStepLimit);
    bepmcoptions.def_readwrite("refine", &Options::refine);
    bepmcoptions.def_readwrite("exploration_heuristic", &Options::explorationHeuristic);
    bepmcoptions.def_readwrite("skip_heuristic_schedulers", &Options::skipHeuristicSchedulers);
    bepmcoptions.def_readwrite("interactive_unfolding", &Options::interactiveUnfolding);
    bepmcoptions.def_readwrite("cut_zero_gap", &Options::cutZeroGap);
    
    py::class_<typename storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::Result> bepmcres(m, ("BeliefExplorationPomdpModelCheckerResult" + vtSuffix).c_str());
    bepmcres.def_readonly("induced_mc_from_scheduler", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::Result::schedulerAsMarkovChain);
    bepmcres.def_readonly("cutoff_schedulers", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::Result::cutoffSchedulers);
    bepmcres.def_readonly("lower_bound", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::Result::lowerBound);
    bepmcres.def_readonly("upper_bound", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::Result::upperBound);

    m.def("create_interactive_mc", &storm::pomdp::api::createInteractiveUnfoldingModelChecker<ValueType>, py::arg("env"), py::arg("pomdp"), py::arg("use_clipping"));
}

template void define_verification<double>(py::module& m, std::string const& vtSuffix);