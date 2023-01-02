#include "verification.h"
#include <storm-pomdp/api/verification.h>

template<typename ValueType>
void define_verification(py::module& m, std::string const& vtSuffix) {
    typedef storm::pomdp::modelchecker::BeliefExplorationPomdpModelCheckerOptions<ValueType> Options;
    //typedef storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>> BeliefExplorationPomdpModelChecker;
    py::class_<typename storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>> bepmc(m, ("BeliefExplorationModelChecker" + vtSuffix).c_str());
    bepmc.def(py::init<std::shared_ptr<storm::models::sparse::Pomdp<ValueType>>, Options>(), py::arg("model"), py::arg("options"));
    bepmc.def("check", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::check, py::arg("task"), py::arg("values"), py::call_guard<py::gil_scoped_release>());
    bepmc.def("pause_unfolding", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::pauseUnfolding);
    bepmc.def("continue_unfolding", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::continueUnfolding);
    bepmc.def("terminate_unfolding", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::terminateUnfolding);
    bepmc.def("is_result_ready", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::isResultReady);
    bepmc.def("is_exploring", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::isExploring);
    bepmc.def("get_interactive_result", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::getInteractiveResult);
    bepmc.def("get_status", &storm::pomdp::modelchecker::BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<ValueType>>::getStatus);

    //py::enum_<BeliefExplorationPomdpModelChecker::Status>(bepmc, "Status")
    //    .value("Uninitialized", BeliefExplorationPomdpModelChecker::Status::Uninitialized)
    //    .value("Exploring", BeliefExplorationPomdpModelChecker::Status::Exploring)
    //    .value("ModelExplorationFinished", BeliefExplorationPomdpModelChecker::Status::ModelExplorationFinished)
    //    .value("ResultAvailable", BeliefExplorationPomdpModelChecker::Status::ResultAvailable)
    //    .value("Terminated", BeliefExplorationPomdpModelChecker::Status::Terminated);

    py::class_<Options> bepmcoptions(m, ("BeliefExplorationModelCheckerOptions" + vtSuffix).c_str());
    bepmcoptions.def(py::init<bool, bool>(), py::arg("discretize"), py::arg("unfold"));
    bepmcoptions.def_readwrite("use_explicit_cutoff", &Options::useExplicitCutoff);
    bepmcoptions.def_readwrite("size_threshold_init", &Options::sizeThresholdInit);
    bepmcoptions.def_readwrite("use_grid_clipping", &Options::useGridClipping);
    bepmcoptions.def_readwrite("exploration_time_limit", &Options::explorationTimeLimit);
    bepmcoptions.def_readwrite("clipping_threshold_init", &Options::clippingThresholdInit);
    bepmcoptions.def_readwrite("clipping_grid_res", &Options::clippingGridRes);
    bepmcoptions.def_readwrite("gap_threshold_init", &Options::gapThresholdInit);
    bepmcoptions.def_readwrite("size_threshold_factor", &Options::sizeThresholdFactor);
    bepmcoptions.def_readwrite("refine_precision", &Options::refinePrecision);
    bepmcoptions.def_readwrite("refine_step_limit", &Options::refineStepLimit);
    bepmcoptions.def_readwrite("refine", &Options::refine);
    bepmcoptions.def_readwrite("exploration_heuristic", &Options::explorationHeuristic);
    bepmcoptions.def_readwrite("preproc_minmax_method", &Options::preProcMinMaxMethod);
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