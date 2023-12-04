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
        .def_property_readonly("product_state_to_state", [](synthesis::ProductPomdpFsc<double>& m) {return m.product_state_to_state;} )
        ;

    m.def("randomize_action_variant", &synthesis::randomizeActionVariant<double>);

    py::class_<synthesis::ProductPomdpRandomizedFsc<double>>(m, "ProductPomdpRandomizedFsc")
        .def(
            py::init<storm::models::sparse::Model<double> const&, std::vector<uint32_t>, uint64_t, std::vector<uint64_t>>(),
            py::arg("quotient"), py::arg("state_to_obs_class"), py::arg("num_actions"), py::arg("choice_to_action")
        )
        .def("apply_fsc", &synthesis::ProductPomdpRandomizedFsc<double>::applyFsc, py::arg("action_function"), py::arg("udate_function"))
        .def_property_readonly("product", [](synthesis::ProductPomdpRandomizedFsc<double>& m) {return m.product;} )
        .def_property_readonly("product_choice_to_choice", [](synthesis::ProductPomdpRandomizedFsc<double>& m) {return m.product_choice_to_choice;} )
        .def_property_readonly("product_state_to_state", [](synthesis::ProductPomdpRandomizedFsc<double>& m) {return m.product_state_to_state;} )
        ;

    py::class_<synthesis::GameAbstractionSolver<double>>(m, "GameAbstractionSolver")
        .def(
            py::init<storm::models::sparse::Model<double> const&, uint64_t, std::vector<uint64_t> const&, std::string const&>(),
            py::arg("quotient"), py::arg("quoitent_num_actions"), py::arg("choice_to_action"), py::arg("target_label")
        )
        .def("solve", &synthesis::GameAbstractionSolver<double>::solve)
        .def_property_readonly("solution_state_values", [](synthesis::GameAbstractionSolver<double>& solver) {return solver.solution_state_values;})
        .def_property_readonly("solution_value", [](synthesis::GameAbstractionSolver<double>& solver) {return solver.solution_value;})
        .def_property_readonly("solution_state_to_player1_action", [](synthesis::GameAbstractionSolver<double>& solver) {return solver.solution_state_to_player1_action;})
        .def_property_readonly("solution_all_choices", [](synthesis::GameAbstractionSolver<double>& solver) {return solver.solution_all_choices;})
        .def_property_readonly("solution_reachable_choices", [](synthesis::GameAbstractionSolver<double>& solver) {return solver.solution_reachable_choices;})
        .def("enable_profiling", &synthesis::GameAbstractionSolver<double>::enableProfiling)
        .def("print_profiling", &synthesis::GameAbstractionSolver<double>::printProfiling)
        ;
}
