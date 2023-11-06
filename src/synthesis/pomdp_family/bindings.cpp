#include "../synthesis.h"

#include "JaniChoices.h"
#include "ObservationEvaluator.h"
#include "ProductPomdpFsc.h"
#include "ProductPomdpRandomizedFsc.h"
#include "GameAbstractionSolver.h"

void bindings_pomdp_family(py::module& m) {

    m.def("add_choice_labels_from_jani", &synthesis::addChoiceLabelsFromJani<double>);

    py::class_<synthesis::ObservationEvaluator<double>>(m, "ObservationEvaluator")
        .def(py::init<storm::prism::Program &,storm::models::sparse::Model<double> const& >(), py::arg("prism"), py::arg("model"))
        .def_property_readonly("num_obs_expressions", [](synthesis::ObservationEvaluator<double>& e) {return e.num_obs_expressions;} )
        .def_property_readonly("obs_expr_label", [](synthesis::ObservationEvaluator<double>& e) {return e.obs_expr_label;} )
        .def_property_readonly("obs_expr_is_boolean", [](synthesis::ObservationEvaluator<double>& e) {return e.obs_expr_is_boolean;} )
        .def_property_readonly("num_obs_classes", [](synthesis::ObservationEvaluator<double>& e) {return e.num_obs_classes;} )
        .def_property_readonly("state_to_obs_class", [](synthesis::ObservationEvaluator<double>& e) {return e.state_to_obs_class;} )
        .def("obs_class_value", &synthesis::ObservationEvaluator<double>::observationClassValue, py::arg("obs_class"), py::arg("obs_expr"))
        .def("add_observations_to_submdp", &synthesis::ObservationEvaluator<double>::addObservationsToSubMdp, py::arg("mdp"), py::arg("state_sub_to_full"))
        ;

    py::class_<synthesis::ProductPomdpFsc<double>>(m, "ProductPomdpFsc")
        .def(
            py::init<storm::models::sparse::Model<double> const&, std::vector<uint32_t>, uint64_t, std::vector<uint64_t>>(),
            py::arg("quotient"), py::arg("state_to_obs_class"), py::arg("num_actions"), py::arg("choice_to_action")
        )
        .def("apply_fsc", &synthesis::ProductPomdpFsc<double>::applyFsc, py::arg("action_function"), py::arg("udate_function"))
        .def_property_readonly("product", [](synthesis::ProductPomdpFsc<double>& m) {return m.product;} )
        .def_property_readonly("product_choice_to_choice", [](synthesis::ProductPomdpFsc<double>& m) {return m.product_choice_to_choice;} )
        ;

    py::class_<synthesis::ProductPomdpRandomizedFsc<double>>(m, "ProductPomdpRandomizedFsc")
        .def(
            py::init<storm::models::sparse::Model<double> const&, std::vector<uint32_t>, uint64_t, std::vector<uint64_t>>(),
            py::arg("quotient"), py::arg("state_to_obs_class"), py::arg("num_actions"), py::arg("choice_to_action")
        )
        .def("apply_fsc", &synthesis::ProductPomdpRandomizedFsc<double>::applyFsc, py::arg("action_function"), py::arg("udate_function"))
        .def_property_readonly("product", [](synthesis::ProductPomdpRandomizedFsc<double>& m) {return m.product;} )
        .def_property_readonly("product_choice_to_choice", [](synthesis::ProductPomdpRandomizedFsc<double>& m) {return m.product_choice_to_choice;} )
        ;

    py::class_<synthesis::GameAbstractionSolver<double>>(m, "GameAbstractionSolver")
        .def(
            py::init<storm::models::sparse::Model<double> const&, std::vector<uint64_t> const&,
            std::vector<std::vector<uint32_t>> const&, std::string const&>(),
            py::arg("quotient"), py::arg("choice_to_action"), py::arg("state_to_actions"), py::arg("target_label")
        )
        .def("solve", &synthesis::GameAbstractionSolver<double>::solve)
        .def_property_readonly("player1_state_values", [](synthesis::GameAbstractionSolver<double>& solver) {return solver.player1_state_values;})
        .def_property_readonly("player1_choices", [](synthesis::GameAbstractionSolver<double>& solver) {return solver.player1_choices;})
        .def_property_readonly("player2_choices", [](synthesis::GameAbstractionSolver<double>& solver) {return solver.player2_choices;})
        ;
}
