#include "synthesis.h"

#include "storm-synthesis/pomdp/SubPomdpBuilder.h"

void define_pomdp_builder(py::module& m) {

    py::class_<storm::synthesis::SubPomdpBuilder, std::shared_ptr<storm::synthesis::SubPomdpBuilder>>(m, "SubPomdpBuilder")
        .def(py::init<storm::models::sparse::Pomdp<double> const&, std::string const&, std::string const&>())
        .def("set_discount_factor", &storm::synthesis::SubPomdpBuilder::setDiscountFactor)
        .def("set_relevant_observations", &storm::synthesis::SubPomdpBuilder::setRelevantObservations)
        .def_property_readonly("relevant_states", [](storm::synthesis::SubPomdpBuilder& builder) {return builder.relevant_states;})
        .def_property_readonly("frontier_states", [](storm::synthesis::SubPomdpBuilder& builder) {return builder.frontier_states;})
        .def("restrict_pomdp", &storm::synthesis::SubPomdpBuilder::restrictPomdp)
        .def_property_readonly("state_sub_to_full", [](storm::synthesis::SubPomdpBuilder& builder) {return builder.state_sub_to_full;})
        .def_property_readonly("state_full_to_sub", [](storm::synthesis::SubPomdpBuilder& builder) {return builder.state_full_to_sub;})
        ;
}

