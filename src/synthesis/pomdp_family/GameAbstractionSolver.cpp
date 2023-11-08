#include "GameAbstractionSolver.h"

#include <queue>

namespace synthesis {
        
    template<typename ValueType>
    void print_matrix(storm::storage::SparseMatrix<ValueType> matrix) {
        auto const& row_group_indices = matrix.getRowGroupIndices();
        for(uint64_t state=0; state < matrix.getRowGroupCount(); state++) {
            std::cout << "state " << state << ": " << std::endl;
            for(uint64_t row=row_group_indices[state]; row<row_group_indices[state+1]; row++) {
                for(auto const &entry: matrix.getRow(row)) {
                    std::cout << state << "-> "  << entry.getColumn() << " ["  << entry.getValue() << "];";
                }
                std::cout << std::endl;
            }
        }
        std::cout << "-----" << std::endl;
    }

    template void print_matrix<storm::storage::sparse::state_type>(storm::storage::SparseMatrix<storm::storage::sparse::state_type> matrix);
    template void print_matrix<double>(storm::storage::SparseMatrix<double> matrix);

    template<typename ValueType>
    GameAbstractionSolver<ValueType>::GameAbstractionSolver(
        storm::models::sparse::Model<ValueType> const& quotient,
        uint64_t quotient_num_actions,
        std::vector<uint64_t> const& choice_to_action,
        std::vector<std::vector<uint64_t>> const& state_to_actions,
        std::string const& target_label
    ) : quotient_num_actions(quotient_num_actions) {

        this->quotient_num_states = quotient.getNumberOfStates();
        this->quotient_initial_state = *(quotient.getInitialStates().begin());
        // copy state_to_action (why storing reference doesn't work?)
        this->state_to_actions.resize(state_to_actions.size());
        for(uint64_t state = 0; state < state_to_actions.size(); state++) {
            for(auto action: state_to_actions[state]) {
                this->state_to_actions[state].push_back(action);
            }
        }
        this->collectQuotientChoiceDestinations(quotient);

        this->setupSolverEnvironment();
        this->solution_choices = storm::storage::BitVector(quotient.getNumberOfChoices());

        this->buildStateSpace();
        this->buildPlayer1Matrix();
        this->buildPlayer2Matrix(quotient,choice_to_action,target_label);

        /*std::cout << "Player 1 matrix:" << std::endl;
        synthesis::print_matrix<storm::storage::sparse::state_type>(this->player1_matrix);
        std::cout << "Player 2 matrix:" << std::endl;
        synthesis::print_matrix<double>(this->player2_matrix_full);*/
    }

    template<typename ValueType>
    void GameAbstractionSolver<ValueType>::collectQuotientChoiceDestinations(
        storm::models::sparse::Model<ValueType> const& quotient) {
        auto quotient_num_choices = quotient.getNumberOfChoices();
        this->quotient_choice_destinations.resize(quotient_num_choices);
        for(uint64_t choice=0; choice<quotient_num_choices; choice++) {
            for(auto const &entry: quotient.getTransitionMatrix().getRow(choice)) {
                this->quotient_choice_destinations[choice].push_back(entry.getColumn());
            }
        }
    }


    template<typename ValueType>
    void GameAbstractionSolver<ValueType>::buildStateSpace() {
        // build states of Player 1, including fresh target state
        this->player1_num_states = this->quotient_num_states;
        this->player1_target_state = this->player1_num_states++;

        // build states of Player 2, including fresh target state
        this->player2_num_states = 0;
        this->state_action_to_player2_state.resize(this->quotient_num_states);
        for(uint64_t state=0; state < quotient_num_states; state++) {
            this->state_action_to_player2_state[state].resize(this->quotient_num_actions);
            for(auto action: this->state_to_actions[state]) {
                this->state_action_to_player2_state[state][action] = this->player2_num_states++;
            }
        }
        this->player2_target_state = this->player2_num_states++;
    }

    
    template<typename ValueType>
    void GameAbstractionSolver<ValueType>::setupSolverEnvironment() {
        this->env.solver().game().setPrecision(storm::utility::convertNumber<storm::RationalNumber>(1e-8));

        // value iteration
        // this->env.solver().game().setMethod(storm::solver::GameMethod::ValueIteration);
        
        // policy iteration
        this->env.solver().game().setMethod(storm::solver::GameMethod::PolicyIteration);
        this->env.solver().setLinearEquationSolverType(storm::solver::EquationSolverType::Native);
        this->env.solver().native().setMethod(storm::solver::NativeLinearEquationSolverMethod::Jacobi);
        this->env.solver().setLinearEquationSolverPrecision(env.solver().game().getPrecision());
    }

    
    template<typename ValueType>
    void GameAbstractionSolver<ValueType>::buildPlayer1Matrix() {
        storm::storage::SparseMatrixBuilder<storm::storage::sparse::state_type> player1_matrix_builder(0,0,0,false,true);
        this->player1_num_rows = 0;
        for(uint64_t state=0; state < quotient_num_states; state++) {
            player1_matrix_builder.newRowGroup(this->player1_num_rows);
            for(auto action: this->state_to_actions[state]) {
                auto player2_state = this->state_action_to_player2_state[state][action];
                player1_matrix_builder.addNextValue(this->player1_num_rows,player2_state,1);
                this->player1_num_rows++;
            }
        }
        player1_matrix_builder.newRowGroup(this->player1_num_rows);
        player1_matrix_builder.addNextValue(this->player1_num_rows,this->player2_target_state,1);
        this->player1_num_rows++;
        this->player1_matrix = player1_matrix_builder.build();
    }

    
    template<typename ValueType>
    void GameAbstractionSolver<ValueType>::buildPlayer2Matrix(
        storm::models::sparse::Model<ValueType> const& quotient,
        std::vector<uint64_t> const& choice_to_action,
        std::string const& target_label
    ) {
        // identify target states
        storm::storage::BitVector state_is_target(quotient_num_states);
        for(auto state: quotient.getStateLabeling().getStates(target_label)) {
            state_is_target.set(state,true);
        }

        // map state-action pairs to used choices
        /** For each state-action pair, a list of choices that represent variants of this action in the state. */
        std::vector<std::vector<std::vector<uint64_t>>> state_action_to_choices(this->quotient_num_states);
        auto const& quotient_row_group_indices = quotient.getTransitionMatrix().getRowGroupIndices();
        for(uint64_t state=0; state < quotient_num_states; state++) {
            state_action_to_choices[state].resize(this->quotient_num_actions);
            for(auto choice = quotient_row_group_indices[state]; choice < quotient_row_group_indices[state+1]; choice++) {
                auto action = choice_to_action[choice];
                state_action_to_choices[state][action].push_back(choice);
            }
        }

        storm::storage::SparseMatrixBuilder<ValueType> player2_matrix_builder(0, 0, 0, false, true);
        this->quotient_choice_to_player2_choice.resize(quotient.getNumberOfChoices());
        this->player2_num_rows = 0;
        for(uint64_t state=0; state < quotient_num_states; state++) {
            for(auto action: this->state_to_actions[state]) {
                player2_matrix_builder.newRowGroup(this->player2_num_rows);
                for(auto choice: state_action_to_choices[state][action]) {
                    this->quotient_choice_to_player2_choice[choice] = this->player2_num_rows;
                    this->player2_choice_to_quotient_choice.push_back(choice);
                    if(state_is_target[state]) {
                        // target state, transition to the target state of Player 1 and gain unit reward
                        player2_matrix_builder.addNextValue(this->player2_num_rows,player1_target_state,1);
                        this->player2_row_rewards_full.push_back(1);
                    } else {
                        // create a copy with zero reward
                        for(auto const &entry: quotient.getTransitionMatrix().getRow(choice)) {
                            player2_matrix_builder.addNextValue(this->player2_num_rows,entry.getColumn(),entry.getValue());
                        }    
                        this->player2_row_rewards_full.push_back(0);
                    }
                    this->player2_num_rows++;
                }
            }
        }
        player2_matrix_builder.newRowGroup(this->player2_num_rows);
        player2_matrix_builder.addNextValue(this->player2_num_rows,player1_target_state,1);
        this->player2_choice_to_quotient_choice.push_back(quotient.getNumberOfChoices());
        this->player2_row_rewards_full.push_back(0);
        this->player2_num_rows++;
        this->player2_matrix_full = player2_matrix_builder.build();
    }



    template<typename ValueType>
    storm::OptimizationDirection GameAbstractionSolver<ValueType>::getOptimizationDirection(bool maximizing) {
        return maximizing ? storm::OptimizationDirection::Maximize : storm::OptimizationDirection::Minimize;
    }

    
    template<typename ValueType>
    void GameAbstractionSolver<ValueType>::solve(
        storm::storage::BitVector quotient_choices_mask,
        bool player1_maximizing,
        bool player2_maximizing
    ) {

        // prepare mask for Player2's rows, don't forget about the target state
        storm::storage::BitVector player2_choices_mask(this->player2_num_rows);
        for(auto quotient_choice: quotient_choices_mask) {
            auto player2_choice = this->quotient_choice_to_player2_choice[quotient_choice];
            player2_choices_mask.set(player2_choice,true);
        }
        auto player2_target_state_row = this->player2_num_rows-1;
        player2_choices_mask.set(player2_target_state_row,true);
        std::cout << "quotient mask has " << quotient_choices_mask.size() << std::endl;
        std::cout << "player 2 mask has " << player2_choices_mask.size() << std::endl;

        

        auto player2_matrix = this->player2_matrix_full.filterEntries(player2_choices_mask);
        std::cout << "player 2 matrix (full) has " << this->player2_matrix_full.getRowCount() << std::endl;
        std::cout << "player 2 matrix has " << player2_matrix.getRowCount() << std::endl;

        // map player2 choices back to quotients
        for(auto player2_choice: player2_choices_mask) {
            if(player2_choice == player2_target_state_row) {
                continue;
            }
            auto quotient_choice = this->player2_choice_to_quotient_choice[player2_choice];
            if(not quotient_choices_mask[quotient_choice]) {
                std::cout << "player 2 maps to choice " << quotient_choice << " that is not in the mask" << std::endl;
                exit(1);
            }
        }

        // solve the game
        auto solver = storm::solver::GameSolverFactory<ValueType>().create(env, this->player1_matrix, player2_matrix);
        solver->setTrackSchedulers(true);
        auto player1_direction = this->getOptimizationDirection(player1_maximizing);
        auto player2_direction = this->getOptimizationDirection(player2_maximizing);
        this->solution_state_values = std::vector<double>(this->quotient_num_states+1,0);
        solver->solveGame(this->env, player1_direction, player2_direction, this->solution_state_values, this->player2_row_rewards_full);
        auto player1_choices = solver->getPlayer1SchedulerChoices();
        auto player2_choices = solver->getPlayer2SchedulerChoices();
        for(uint64_t player2_state = 0; player2_state<player2_num_states; player2_state++) {
            auto player2_choice = player2_matrix.getRowGroupIndices()[player2_state]+player2_choices[player2_state];
            std::cout << "state=" << player2_state << ", player2_choice=" << player2_choice << " : ";
            for(auto const& entry: player2_matrix.getRow(player2_choice)) {
                std::cout << entry.getColumn() << ", ";
            }
            std::cout << std::endl;
        }

        /*std::cout << "Player 2 row rewards: " << std::endl;
        for(auto reward: this->player2_row_rewards_full) {
            std::cout << reward << ",";
        }
        std::cout << std::endl;*/

        /*std::cout << "Player 1 choices: " << std::endl;
        for(auto choice: player1_choices) {
            std::cout << choice << ",";
        }
        std::cout << std::endl;
        std::cout << "Player 2 choices: " << std::endl;
        for(auto choice: player2_choices) {
            std::cout << choice << ",";
        }
        std::cout << std::endl;*/
        
        this->solution_state_values.resize(this->quotient_num_states);
        this->solution_value = this->solution_state_values[this->quotient_initial_state];


        // map game result to the selection of the (reachable) choices in the quotient
        // collect all reachable solution choices
        
        auto const& player2_matrix_row_group_indices = player2_matrix.getRowGroupIndices();
        std::cout << player2_matrix.getRowGroupSize(1) << std::endl;
        this->solution_choices.clear();
        storm::storage::BitVector state_is_encountered(this->quotient_num_states);
        std::queue<uint64_t> unexplored_states;
        unexplored_states.push(this->quotient_initial_state);
        state_is_encountered.set(this->quotient_initial_state,true);
        while(not unexplored_states.empty()) {
            auto state = unexplored_states.front();
            unexplored_states.pop();

            // get action selected by Player 1
            auto action_index = player1_choices[state];
            auto action = this->state_to_actions[state][action_index];
            auto player2_state = this->state_action_to_player2_state[state][action];

            // find the action variant selected by Player 2
            if(player2_choices[player2_state] >= player2_matrix.getRowGroupSize(player2_state)) {
                std::cout << "invalid choice for player 2" << std::endl;
                exit(1);
            }
            auto player2_choice = player2_matrix_row_group_indices[player2_state]+player2_choices[player2_state];
            if(not player2_choices_mask[player2_choice]) {
                std::cout << "state " << state << std::endl;
                std::cout << "player2_state " << player2_state << std::endl;
                std::cout << "player2_choice " << player2_choice << std::endl;
                std::cout << "is not in the mask" << std::endl;
                exit(1);
            }

            auto quotient_choice = this->player2_choice_to_quotient_choice[player2_choice];
            if(not quotient_choices_mask[quotient_choice]) {
                std::cout << "state " << state << std::endl;
                std::cout << "player2_state " << player2_state << std::endl;
                std::cout << "player2_choice " << player2_choice << std::endl;
                std::cout << "quotient_choice " << quotient_choice << std::endl;
                std::cout << "quotient choice " << quotient_choice << " is not in the mask" << std::endl;
                exit(1);
            }
            this->solution_choices.set(quotient_choice,true);

            // add unexplored destinations of this choice
            for(auto dst: this->quotient_choice_destinations[quotient_choice]) {
                if(not state_is_encountered[dst]) {
                    unexplored_states.push(dst);
                    state_is_encountered.set(dst,true);
                }
            }
        }


        this->solution_state_to_player1_action = std::vector<uint64_t>(this->quotient_num_states);
        for(uint64_t state=0; state<this->quotient_num_states; state++) {
            auto action_index = player1_choices[state];
            auto action = this->state_to_actions[state][action_index];
            this->solution_state_to_player1_action[state] = action;
        }

    }


    template class GameAbstractionSolver<double>;
}