#pragma once

#include "storm/solver/GameSolver.h"
#include "storm/environment/solver/GameSolverEnvironment.h"

namespace synthesis {

    template<typename ValueType>
    class GameAbstractionSolver {

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

        /** Solution value of the game. */
        double solution_value;
        /** Choices of the quotient that represent the game solution. */
        storm::storage::BitVector solution_choices;
        // TODO filter unreachable choices


        
    
    private:

        /** Number of states of the quotient, which coincides with the number of states of Player 1. */
        uint64_t quotient_num_states;
        /** Initial state of player 1. */
        uint64_t quotient_initial_state;
        /** For each state of the quotient, a list of actions associated with its rows. */
        std::vector<std::vector<uint64_t>> state_to_actions;
        /** For each state-action pair, a list of choices that represent variants of this action in the state. */
        std::vector<std::vector<std::vector<uint64_t>>> state_action_to_choices;

        /** Solver environment. */
        storm::Environment env;

        /**
         * Player 1 matrix. This player's states are states of the quotient, where in each state s the player has a
         * choice of an action a, which leads to state (s,a) of Player 2.
         */
        storm::storage::SparseMatrix<storm::storage::sparse::state_type> player1_matrix;

        /** For each state-action pair, the corresponding state of Player 2. */
        std::vector<std::vector<uint64_t>> state_action_to_player2_state;
        /**
         * Player 2 matrix. This player's states are pairs (s,a), where s is the state of the quotient and a is the
         * action selected previously by Player 1. In this state (s,a), Player 2 chooses the color of a to be executed.
         */
        storm::storage::SparseMatrix<ValueType> player2_matrix_full;

        
        /** Mapping of the choice of Player 2 to the quotient choice. */
        std::vector<uint64_t> player2_choice_to_quotient_choice;
        /** Mapping of the quotient choice to choice of Player 2. */
        std::vector<uint64_t> quotient_choice_to_player2_choice;

        /** For each choice of Player 2, a reward to obtain: 1 for the deadlock action in target states, 0 otherwise. */
        std::vector<double> player2_row_rewards_full;

        void gameSolverTest();

    };
}