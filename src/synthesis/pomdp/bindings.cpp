#include "../synthesis.h"

#include "SubPomdpBuilder.h"

void bindings_pomdp(py::module& m) {

    py::class_<synthesis::SubPomdpBuilder, std::shared_ptr<synthesis::SubPomdpBuilder>>(m, "SubPomdpBuilder")
        .def(py::init<storm::models::sparse::Pomdp<double> const&, std::string const&, std::string const&>())
        .def("set_discount_factor", &synthesis::SubPomdpBuilder::setDiscountFactor)
        .def("set_relevant_observations", &synthesis::SubPomdpBuilder::setRelevantObservations)
        .def_property_readonly("relevant_states", [](synthesis::SubPomdpBuilder& builder) {return builder.relevant_states;})
        .def_property_readonly("frontier_states", [](synthesis::SubPomdpBuilder& builder) {return builder.frontier_states;})
        .def("restrict_pomdp", &synthesis::SubPomdpBuilder::restrictPomdp)
        .def_property_readonly("state_sub_to_full", [](synthesis::SubPomdpBuilder& builder) {return builder.state_sub_to_full;})
        .def_property_readonly("state_full_to_sub", [](synthesis::SubPomdpBuilder& builder) {return builder.state_full_to_sub;})
        ;
}

