#include "GameAbstractionSolver.h"

namespace synthesis {
        
    template<typename ValueType>
    GameAbstractionSolver<ValueType>::GameAbstractionSolver(
        storm::models::sparse::Model<ValueType> const& quotient,
        std::vector<uint64_t> const& choice_to_action,
        std::vector<std::vector<uint32_t>> const& state_to_actions,
        std::string const& target_label
    ) {
        // setup environment
        env.solver().game().setMethod(storm::solver::GameMethod::ValueIteration);
        env.solver().game().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));

        // identify target states
        storm::storage::BitVector state_is_target(quotient.getNumberOfStates());
        for(auto state: quotient.getStateLabeling().getStates(target_label)) {
            state_is_target.set(state,true);
        }

        // build player1's matrix
        storm::storage::SparseMatrixBuilder<storm::storage::sparse::state_type> player1_matrix_builder(0, 0, 0, false, true);
        uint64_t player1_num_rows = 0;
        for(uint64_t state=0; state < quotient.getNumberOfStates(); state++) {
            player1_matrix_builder.newRowGroup(player1_num_rows);
            for(auto action: state_to_actions[state]) {
                player1_matrix_builder.addNextValue(player1_num_rows,player1_num_rows,1);
                player1_num_rows++;
            }
        }
        this->player1_matrix = player1_matrix_builder.build();

        // build player2's matrix
        storm::storage::SparseMatrixBuilder<ValueType> player2_matrix_builder(0, 0, 0, false, true);
        auto const& row_group_indices = quotient.getTransitionMatrix().getRowGroupIndices();
        uint64_t player2_num_rows = 0;
        for(uint64_t state=0; state < quotient.getNumberOfStates(); state++) {
            player1_matrix_builder.newRowGroup(player1_num_rows);
            for(auto action: state_to_actions[state]) {
                // create a separate state that executes variants of this action
                player2_matrix_builder.newRowGroup(player2_num_rows);
                if(state_is_target[state]) {
                    // target state, add empty row with unit reward
                    this->player2_row_rewards_full.push_back(1);
                    player2_num_rows++;
                    continue;
                }
                for(auto choice = row_group_indices[state]; choice < row_group_indices[state+1]; choice++) {
                    if(choice_to_action[choice] != action) {
                        continue;
                    }
                    // sub-MDP uses this choice: add its copy
                    for(auto const &entry: quotient.getTransitionMatrix().getRow(choice)) {
                        player2_matrix_builder.addNextValue(player2_num_rows,entry.getColumn(),entry.getValue());
                    }    
                    this->player2_row_rewards_full.push_back(0);
                    player2_num_rows++;
                }
            }
        }
        this->player2_matrix_full = player2_matrix_builder.build();
    }


    template<typename ValueType>
    void GameAbstractionSolver<ValueType>::solve(
        storm::storage::BitVector player2_choices_mask,
        bool player1_maximizing,
        bool player2_maximizing
    ) {
        auto player2_matrix = this->player2_matrix_full.filterEntries(player2_choices_mask);
        std::vector<double> player2_row_rewards(player2_choices_mask.size(),0);
        for(auto choice: player2_choices_mask) {
            player2_row_rewards[choice] = this->player2_row_rewards_full[choice];
        }
        
        // prepare result vectors
        this->player1_state_values.resize(this->player1_matrix.getRowGroupCount());
        this->player1_choices.resize(this->player1_matrix.getRowGroupCount());
        this->player2_choices.resize(player2_matrix.getRowGroupCount());

        // solve the game
        storm::solver::GameSolverFactory<ValueType> factory;
        auto solver = factory.create(env, this->player1_matrix, player2_matrix);
        auto player1_direction = player1_maximizing ? storm::OptimizationDirection::Maximize : storm::OptimizationDirection::Minimize;
        auto player2_direction = player2_maximizing ? storm::OptimizationDirection::Maximize : storm::OptimizationDirection::Minimize;
        solver->solveGame(
            this->env, player1_direction, player2_direction,
            this->player1_state_values, player2_row_rewards,
            &(this->player1_choices), &(this->player2_choices)
        );
    }

    
    template class GameAbstractionSolver<double>;
}