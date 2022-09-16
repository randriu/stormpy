#include "synthesis.h"

#include "storm-synthesis/decpomdp/DecPomdp.h"


// Define python bindings
void define_decpomdp(py::module& m) {

    py::class_<storm::synthesis::DecPomdp>(m, "DecPomdp", "dec-POMDP")
        // .def(py::init<std::string const&>(), "constructor.", py::arg("filename"));
        // .def("construct_mdp", &storm::synthesis::PomdpManager<double>::constructMdp, "Unfold memory model (a priori memory update) into the POMDP.")
        .def_property_readonly("num_agents", [](storm::synthesis::DecPomdp& decpomdp) {return decpomdp.num_agents;}, "number of agents")
        .def_property_readonly("num_states", [](storm::synthesis::DecPomdp& decpomdp) {return decpomdp.num_states();}, "number of states")
        .def_property_readonly("row_reward", [](storm::synthesis::DecPomdp& decpomdp) {return decpomdp.row_reward;}, "for each row group, a list of row rewards")
        ;

    m.def("parse_decpomdp", &storm::synthesis::parseDecPomdp,  py::arg("filename"));

}

