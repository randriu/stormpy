#include "synthesis.h"

#include "storm-synthesis/synthesis/StochasticGameSolver.h"

// Define python bindings
void define_games(py::module& m) {

    py::class_<storm::synthesis::StochasticGameSolver<double>, std::shared_ptr<storm::synthesis::StochasticGameSolver<double>>>(m, "StochasticGameSolver")
        .def(py::init<>())
        .def("solve", &storm::synthesis::StochasticGameSolver<double>::solve)
        .def_property_readonly("player1_state_values", [](storm::synthesis::StochasticGameSolver<double>& solver) {return solver.player1_state_values;})
        .def_property_readonly("player1_choices", [](storm::synthesis::StochasticGameSolver<double>& solver) {return solver.player1_choices;})
        .def_property_readonly("player2_choices", [](storm::synthesis::StochasticGameSolver<double>& solver) {return solver.player2_choices;})
        ;
}

