#pragma once

#include "storm/solver/GameSolver.h"
#include "storm/environment/solver/GameSolverEnvironment.h"

namespace synthesis {

    template<typename ValueType>
    class GameAbstractionSolver {

    public:
        
        GameAbstractionSolver(
            storm::models::sparse::Model<ValueType> const& quotient,
            std::vector<uint64_t> const& choice_to_action,
            std::vector<std::vector<uint32_t>> const& state_to_actions,
            std::string const& target_label
        );
        
        void solve(
            storm::storage::BitVector player2_choices_mask,
            bool player1_maximizing,
            bool player2_maximizing
        );

        std::vector<ValueType> player1_state_values;
        std::vector<uint64_t> player1_choices;
        std::vector<uint64_t> player2_choices;
    
    private:
        
        /** Solver environmert. */
        storm::Environment env;

        /**
         * Player 1 matrix. This player's states are states of the quotient, where in each state s the player has a
         * choice of an action a, which leads to state (s,a) of Player 2.
         */
        storm::storage::SparseMatrix<storm::storage::sparse::state_type> player1_matrix;

        /**
         * Player 2 matrix. This player's states are pairs (s,a), where s is the state of the quotient and a is the
         * action selected previously by Player 1. In this state (s,a), Player 2 chooses the color of a to be executed
         */
        storm::storage::SparseMatrix<ValueType> player2_matrix_full;

        /** For each choice of Player 2, a reward to obtain (1 for self-loop in target states). */
        std::vector<double> player2_row_rewards_full;
    };
}