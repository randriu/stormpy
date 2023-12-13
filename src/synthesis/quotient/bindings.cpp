#include "../synthesis.h"

#include "Family.h"
#include "Coloring.h"

void bindings_coloring(py::module& m) {

    py::class_<synthesis::Family>(m, "Family")
        .def(py::init<>(), "Constructor.")
        .def(py::init<synthesis::Family const&>(), "Constructor.", py::arg("other"))
        .def("numHoles", &synthesis::Family::numHoles)
        .def("addHole", &synthesis::Family::addHole)
        
        .def("holeOptions", &synthesis::Family::holeOptions)
        .def("holeOptionsMask", &synthesis::Family::holeOptionsMask)
        .def("holeSetOptions", py::overload_cast<uint64_t, std::vector<uint64_t> const&>(&synthesis::Family::holeSetOptions))
        .def("holeSetOptions", py::overload_cast<uint64_t, storm::storage::BitVector const&>(&synthesis::Family::holeSetOptions))
        .def("holeNumOptions", &synthesis::Family::holeNumOptions)
        .def("holeNumOptionsTotal", &synthesis::Family::holeNumOptionsTotal)
        .def("holeContains", &synthesis::Family::holeContains)
        ;

    py::class_<synthesis::Coloring>(m, "Coloring")
        .def(py::init<synthesis::Family const&, std::vector<uint64_t> const&, std::vector<std::vector<std::pair<uint64_t,uint64_t>>> >(), "Constructor.")
        .def("getChoiceToAssignment", &synthesis::Coloring::getChoiceToAssignment)
        .def("getStateToHoles", &synthesis::Coloring::getStateToHoles)
        .def("getUncoloredChoices", &synthesis::Coloring::getUncoloredChoices)
        .def("selectCompatibleChoices", &synthesis::Coloring::selectCompatibleChoices)
        .def("collectHoleOptions", &synthesis::Coloring::collectHoleOptions)
        ;
}

