#include "synthesis.h"

#include "storm-synthesis/pomdp/SubPomdpBuilder.h"

void define_pomdp_builder(py::module& m) {

    py::class_<storm::synthesis::SubPomdpBuilder, std::shared_ptr<storm::synthesis::SubPomdpBuilder>>(m, "SubPomdpBuilder")
        .def(py::init<storm::models::sparse::Pomdp<double> const&, std::string const&, std::string const&>())
        .def("set_relevant_states", &storm::synthesis::SubPomdpBuilder::setRelevantStates)
        .def("get_horizon_states", &storm::synthesis::SubPomdpBuilder::getHorizonStates)
        .def("restrict_pomdp", &storm::synthesis::SubPomdpBuilder::restrictPomdp)
        ;
}

