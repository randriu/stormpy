#include "synthesis.h"

#include "storm-synthesis/madp/DecPomdp.h"
#include "storm-synthesis/madp/base/DecPOMDPDiscrete.h"


// Define python bindings
void define_decpomdp(py::module& m) {

    py::class_<storm::synthesis::DecPomdp>(m, "DecPomdp", "dec-POMDP")
        // .def(py::init<std::string const&>(), "constructor.", py::arg("filename"));
        // .def("construct_mdp", &storm::synthesis::PomdpManager<double>::constructMdp, "Unfold memory model (a priori memory update) into the POMDP.")
        .def_property_readonly("num_agents", [](storm::synthesis::DecPomdp& decpomdp) {return decpomdp.num_agents;}, "number of agents")
        .def_property_readonly("num_states", [](storm::synthesis::DecPomdp& decpomdp) {return decpomdp.num_states();}, "number of states")
        ;

    m.def("parse_decpomdp", &storm::synthesis::parseDecPomdp,  py::arg("filename"));

}

