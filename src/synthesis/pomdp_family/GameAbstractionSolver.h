#pragma once

#include "src/synthesis/translation/ItemTranslator.h"

#include <storm/solver/GameSolver.h>
#include <storm/environment/Environment.h>
#include <storm/environment/solver/GameSolverEnvironment.h>
#include <storm/environment/solver/NativeSolverEnvironment.h>

namespace synthesis {

    template<typename ValueType>
    class GameAbstractionSolver {

        /*
        - Player 1. States are states of the quotient model + a fresh target state. In each state s Player 1 has a
            choice of an action a, which leads to state (s,a) of Player 2. In their fresh target state, Player 1
            transitions to the fresh target state of Player 2.
        - Player 2. States are pairs (s,a), where s is the state of the quotient and a is the action selected in s by
            Player 1. In state (s,a), Player 2 chooses the color of a to be executed. In state (s,*) where s is the 
            target state of the quotient, Player 2 receives reward 1 and executes action available in this state. In
            their fresh target state, Player 2 loops back to the target state of Player 1.
        */

    public:
        
        /**
         * Create game abstraction solver.
         * @param quotient The quotient MDP. Sub-MDPs from the quotient will be used to construct sub-games.
         * @param quotient_num_action The total number of distinct actions in the quotient.
         * @param choice_to_action For each row of the quotient, the associated action.
         * @param state_to_actions For each state of the quotient, a list of actions associated with its rows.
         * @param target_label Label of the target states.
         */
        GameAbstractionSolver(
            storm::models::sparse::Model<ValueType> const& quotient,
            uint64_t quotient_num_actions,
            std::vector<uint64_t> const& choice_to_action,
            std::vector<std::vector<uint64_t>> const& state_to_actions,
            std::string const& target_label
        );
        
        /**
         * Solve the game induced by the sub-MDP.
         * @param quotient_choices_mask Choices of the quotient that remained in the sub-MDP.
         */
        void solve(
            storm::storage::BitVector quotient_choices_mask,
            bool player1_maximizing,
            bool player2_maximizing
        );

        /** State values for the solution. */
        std::vector<double> solution_state_values;
        /** Solution value of the game. */
        double solution_value;
        /** For each state, an action selected by Player 1. */
        std::vector<uint64_t> solution_state_to_player1_action;
        /** Reachable choices of the quotient that represent the game solution. */
        storm::storage::BitVector solution_choices;
    
    private:

        /** Number of states of the quotient. */
        uint64_t quotient_num_states;
        /** Number of actions of the quotient. */
        uint64_t quotient_num_actions;
        /** Quotient initial states, which is the initial state of Player 1. */
        uint64_t quotient_initial_state;
        /** For each state of the quotient, a list of actions associated with its rows. */
        std::vector<std::vector<uint64_t>> state_to_actions;
        /** For each choice of the quotient, its destinations. */
        std::vector<std::vector<uint64_t>> quotient_choice_destinations;

        /** Solver environment. */
        storm::Environment env;

        /** Number of states of Player 1. */
        uint64_t player1_num_states;
        /** Fresh target state for Player 1. */
        uint64_t player1_target_state;


        /** Number of states of Player 2. */
        uint64_t player2_num_states;
        /** Fresh target state for Player 2. */
        uint64_t player2_target_state;
        /** For each state-action pair, the corresponding state of Player 2. */
        std::vector<std::vector<uint64_t>> state_action_to_player2_state;

        /** Number of rows in the matrix for Player 1. */
        uint64_t player1_num_rows;
        /** Number of rows in the matrix for Player 2. */
        uint64_t player2_num_rows;
        
        /** Player 1 matrix. */
        storm::storage::SparseMatrix<storm::storage::sparse::state_type> player1_matrix;
        
        /** Player 2 matrix. */
        storm::storage::SparseMatrix<ValueType> player2_matrix_full;
        /** Mapping of the choice of Player 2 to the quotient choice. */
        std::vector<uint64_t> player2_choice_to_quotient_choice;
        /** Mapping of the quotient choice to choice of Player 2. */
        std::vector<uint64_t> quotient_choice_to_player2_choice;

        /** For each choice of Player 2, a reward to obtain: 1 for the deadlock action in target states, 0 otherwise. */
        std::vector<double> player2_row_rewards_full;

        
        void setupSolverEnvironment();

        void collectQuotientChoiceDestinations(storm::models::sparse::Model<ValueType> const& quotient);
        
        void buildStateSpace();
        
        void buildPlayer1Matrix();
        
        void buildPlayer2Matrix(
            storm::models::sparse::Model<ValueType> const& quotient,
            std::vector<uint64_t> const& choice_to_action,
            std::string const& target_label);

        storm::OptimizationDirection getOptimizationDirection(bool maximizing);
    };
}