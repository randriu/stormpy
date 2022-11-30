#include "synthesis.h"

#include "storm-synthesis/simulation/SimulatedModel.h"

void define_simulation(py::module& m) {

    py::class_<storm::synthesis::SimulatedModel<double>>(m, "SimulatedModel")
        .def(py::init<storm::models::sparse::Pomdp<double> const&>())
        .def("sample_action", &storm::synthesis::SimulatedModel<double>::sampleAction)
        .def("sample_successor", &storm::synthesis::SimulatedModel<double>::sampleSuccessor)
        .def("state_action_rollout", &storm::synthesis::SimulatedModel<double>::stateActionRollout)
        ;
    
}

