#include "synthesis.h"

#include "storm-synthesis/decpomdp/DecPomdp.h"


// Define python bindings
void define_decpomdp(py::module& m) {

    py::class_<storm::synthesis::DecPomdp>(m, "DecPomdp", "dec-POMDP")
        // .def(py::init<std::string const&>(), "constructor.", py::arg("filename"));
        .def("construct_mdp", &storm::synthesis::DecPomdp::constructMdp)
        .def("construct_pomdp", &storm::synthesis::DecPomdp::constructPomdp)
        .def_property_readonly("num_agents", [](storm::synthesis::DecPomdp& decpomdp) {return decpomdp.num_agents;})
        .def_property_readonly("joint_observations", [](storm::synthesis::DecPomdp& decpomdp) {return decpomdp.joint_observations;})
        .def_property_readonly("agent_observation_labels", [](storm::synthesis::DecPomdp& decpomdp) {return decpomdp.agent_observation_labels;})
        
        .def_property_readonly("reward_model_name", [](storm::synthesis::DecPomdp& decpomdp) {return decpomdp.reward_model_name;})
        .def_property_readonly("reward_minimizing", [](storm::synthesis::DecPomdp& decpomdp) {return decpomdp.reward_minimizing;})
        .def_property_readonly("discount_factor", [](storm::synthesis::DecPomdp& decpomdp) {return decpomdp.discount_factor;})
        
        .def("apply_discount_factor_transformation", &storm::synthesis::DecPomdp::applyDiscountFactorTransformation)
        .def_property_readonly("discount_sink_label", [](storm::synthesis::DecPomdp& decpomdp) {return decpomdp.discount_sink_label;})
        ;

    m.def("parse_decpomdp", &storm::synthesis::parseDecPomdp,  py::arg("filename"));

}

