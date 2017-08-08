#include "expressions.h"
#include "src/helpers.h"

#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/Expression.h"

//Define python bindings
void define_expressions(py::module& m) {

    // ExpressionManager
    py::class_<storm::expressions::ExpressionManager, std::shared_ptr<storm::expressions::ExpressionManager>>(m, "ExpressionManager", "Manages variables for expressions")
        .def(py::init(), "Constructor")
        .def("create_boolean", &storm::expressions::ExpressionManager::boolean, py::arg("boolean"), "Create expression from boolean")
        .def("create_integer", &storm::expressions::ExpressionManager::integer, py::arg("integer"), "Create expression from integer number")
        .def("create_rational", [](storm::expressions::ExpressionManager const& manager, storm::RationalNumber number) {
                return manager.rational(number);
            }, py::arg("rational"), "Create expression from rational number")
    ;

    // Variable
    py::class_<storm::expressions::Variable, std::shared_ptr<storm::expressions::Variable>>(m, "Variable", "Represents a variable")
        .def_property_readonly("name", &storm::expressions::Variable::getName, "Variable name")
        .def("has_boolean_type", &storm::expressions::Variable::hasBooleanType, "Check if the variable is of boolean type")
        .def("has_integer_type", &storm::expressions::Variable::hasIntegerType, "Check if the variable is of integer type")
        .def("has_rational_type", &storm::expressions::Variable::hasRationalType, "Check if the variable is of rational type")
        .def("has_numerical_type", &storm::expressions::Variable::hasNumericalType, "Check if the variable is of numerical type")
        .def("has_bitvector_type", &storm::expressions::Variable::hasBitVectorType, "Check if the variable is of bitvector type")
        .def("get_expression", &storm::expressions::Variable::getExpression, "Get expression from variable")
    ;

    // Expression
    py::class_<storm::expressions::Expression, std::shared_ptr<storm::expressions::Expression>>(m, "Expression", "Holds an expression")
        .def("contains_variables", &storm::expressions::Expression::containsVariables, "Check if the expression contains variables.")
        .def("is_literal", &storm::expressions::Expression::isLiteral, "Check if the expression is a literal")
        .def("has_boolean_type", &storm::expressions::Expression::hasBooleanType, "Check if the expression is a boolean")
        .def("has_integer_type", &storm::expressions::Expression::hasIntegerType, "Check if the expression is an integer")
        .def("has_rational_type", &storm::expressions::Expression::hasRationalType, "Check if the expression is a rational")
        .def("__str__", &streamToString<storm::expressions::Expression>)
    ;

}