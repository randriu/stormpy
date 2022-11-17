#include "synthesis.h"

#include "storm-synthesis/pomdp/SubPomdpBuilder.h"

void define_pomdp_builder(py::module& m) {

    py::class_<storm::synthesis::SubPomdpBuilder, std::shared_ptr<storm::synthesis::SubPomdpBuilder>>(m, "SubPomdpBuilder")
        .def(py::init<storm::models::sparse::Pomdp<double> const&, storm::logic::Formula const&>())
        .def("collect_horizon", &storm::synthesis::SubPomdpBuilder::collectHorizon)
        .def("restrict_pomdp", &storm::synthesis::SubPomdpBuilder::restrictPomdp)
        // .def_property_readonly("row_memory_option", [](storm::synthesis::PomdpManager<double>& manager) {return manager.row_memory_option;}, "TODO")
        ;
}

