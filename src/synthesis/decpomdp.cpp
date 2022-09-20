#include "synthesis.h"

#include "storm-synthesis/decpomdp/DecPomdp.h"


// Define python bindings
void define_decpomdp(py::module& m) {

    py::class_<storm::synthesis::DecPomdp>(m, "DecPomdp", "dec-POMDP")
        // .def(py::init<std::string const&>(), "constructor.", py::arg("filename"));
        // .def("apply_discount_factor", &storm::synthesis::DecPomdp::applyDiscountFactor, "apply the discount factor", py::arg("discount"))
        .def_property_readonly("num_agents", [](storm::synthesis::DecPomdp& decpomdp) {return decpomdp.num_agents;}, "number of agents")
        .def_property_readonly("num_states", [](storm::synthesis::DecPomdp& decpomdp) {return decpomdp.num_states();}, "number of states")
        .def_property_readonly("transition_matrix", [](storm::synthesis::DecPomdp& decpomdp) {return decpomdp.transition_matrix;}, "transition matrix")
        .def_property_readonly("discount", [](storm::synthesis::DecPomdp& decpomdp) {return decpomdp.discount;}, "discount factor")
        .def_property_readonly("reward_minimizing", [](storm::synthesis::DecPomdp& decpomdp) {return decpomdp.reward_minimizing;}, "if True, the reward value is interpreted as a cost")
        .def_property_readonly("row_reward", [](storm::synthesis::DecPomdp& decpomdp) {return decpomdp.row_reward;}, "for each row group, a list of row rewards")
        ;

    m.def("parse_decpomdp", &storm::synthesis::parseDecPomdp,  py::arg("filename"));

}

