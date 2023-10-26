#include "QuotientPomdpManager.h"

#include "storm/exceptions/NotSupportedException.h"

namespace synthesis {

    template<typename ValueType>
    QuotientPomdpManager<ValueType>::QuotientPomdpManager(
        storm::models::sparse::Model<ValueType> const& quotient,
        std::vector<uint32_t> state_to_obs_class,
        uint64_t num_actions,
        std::vector<uint64_t> choice_to_action
    ) : quotient(quotient), state_to_obs_class(state_to_obs_class),
        num_actions(num_actions), choice_to_action(choice_to_action) {
        
        this->state_action_choices.resize(this->quotient.getNumberOfStates());
        auto const& row_group_indices = this->quotient.getTransitionMatrix().getRowGroupIndices();
        for(uint64_t state = 0; state < this->quotient.getNumberOfStates(); state++) {
            this->state_action_choices[state].resize(this->num_actions);
            for (uint64_t row = row_group_indices[state]; row < row_group_indices[state+1]; row++) {
                uint64_t action = this->choice_to_action[row];
                this->state_action_choices[state][action].insert(row);
            }
        }
    }

    
    template<typename ValueType>
    uint64_t QuotientPomdpManager<ValueType>::productNumberOfStates() {
        return this->product_state_to_state_memory.size();
    }

    template<typename ValueType>
    uint64_t QuotientPomdpManager<ValueType>::productNumberOfChoices() {
        return this->product_choice_to_choice_memory.size();
    }
    
    template<typename ValueType>
    uint64_t QuotientPomdpManager<ValueType>::mapStateMemory(uint64_t state, uint64_t memory) {
        if(this->state_memory_registered[state][memory]) {
            return this->state_memory_to_product_state[state][memory];
        }
        auto new_product_state = this->productNumberOfStates();
        this->state_memory_to_product_state[state][memory] = new_product_state;
        this->product_state_to_state_memory.push_back(std::make_pair(state,memory));
        this->state_memory_registered[state].set(memory,true);
        return new_product_state;
    }
    
    template<typename ValueType>
    void QuotientPomdpManager<ValueType>::buildStateSpace(
        uint64_t num_nodes,
        std::vector<std::vector<uint64_t>> action_function,
        std::vector<std::vector<uint64_t>> update_function
    ) {
        uint64_t quotient_num_states = this->quotient.getNumberOfStates();
        this->state_memory_registered.resize(quotient_num_states);
        this->state_memory_to_product_state.resize(quotient_num_states);
        for(uint64_t state = 0; state < quotient_num_states; state++) {
            this->state_memory_registered[state] = storm::storage::BitVector(num_nodes);
            this->state_memory_to_product_state[state].resize(num_nodes);
        }
        this->product_choice_to_choice.clear();

        uint64_t initial_state = *(this->quotient.getInitialStates().begin());
        uint64_t initial_memory = 0;
        auto product_state = this->mapStateMemory(initial_state,initial_memory);
        while(true) {
            this->product_state_row_group_start.push_back(this->productNumberOfChoices());
            auto[state,memory] = this->product_state_to_state_memory[product_state];
            auto observation = this->state_to_obs_class[state];
            auto action = action_function[memory][observation];
            auto memory_dst = update_function[memory][observation];
            for(auto choice: this->state_action_choices[state][action]) {
                this->product_choice_to_choice_memory.push_back(std::make_pair(choice,memory_dst));
                this->product_choice_to_choice.push_back(choice);
                for(auto const &entry: this->quotient.getTransitionMatrix().getRow(choice)) {
                    auto state_dst = entry.getColumn();
                    this->mapStateMemory(state_dst,memory_dst);
                }
            }
            product_state++;
            if(product_state >= this->productNumberOfStates()) {
                break;
            }
        }
        this->product_state_row_group_start.push_back(this->productNumberOfChoices());
    }

    
    template<typename ValueType>
    storm::models::sparse::StateLabeling QuotientPomdpManager<ValueType>::buildStateLabeling() {
        storm::models::sparse::StateLabeling product_labeling(this->productNumberOfStates());
        for (auto const& label : this->quotient.getStateLabeling().getLabels()) {
            product_labeling.addLabel(label, storm::storage::BitVector(this->productNumberOfStates(), false));
        }
        for(uint64_t product_state = 0; product_state < this->productNumberOfStates(); product_state++) {
            auto[state,memory] = this->product_state_to_state_memory[product_state];
            for (auto const& label : this->quotient.getStateLabeling().getLabelsOfState(state)) {
                if(label == "init" and memory != 0) {
                    // init label is only assigned to states with the initial memory state
                    continue;
                }
                product_labeling.addLabelToState(label,product_state);
            }
        }
        return product_labeling;
    }

    template<typename ValueType>
    storm::storage::SparseMatrix<ValueType> QuotientPomdpManager<ValueType>::buildTransitionMatrix(
    ) {
        storm::storage::SparseMatrixBuilder<ValueType> builder(0, 0, 0, false, true, 0);
        for(uint64_t product_state = 0; product_state < this->productNumberOfStates(); product_state++) {
            builder.newRowGroup(this->product_state_row_group_start[product_state]);
            for(
                auto product_choice = this->product_state_row_group_start[product_state];
                product_choice<this->product_state_row_group_start[product_state+1];
                product_choice++
            ) {
                auto[choice,memory_dst] = this->product_choice_to_choice_memory[product_choice];
                for(auto const &entry: this->quotient.getTransitionMatrix().getRow(choice)) {
                    auto product_dst = this->mapStateMemory(entry.getColumn(),memory_dst);
                    builder.addNextValue(product_choice, product_dst, entry.getValue());
                }
            }
        }

        return builder.build();
    }

    template<typename ValueType>
    storm::models::sparse::ChoiceLabeling QuotientPomdpManager<ValueType>::buildChoiceLabeling() {
        storm::models::sparse::ChoiceLabeling product_labeling(this->productNumberOfChoices());
        for (auto const& label : this->quotient.getChoiceLabeling().getLabels()) {
            product_labeling.addLabel(label, storm::storage::BitVector(this->productNumberOfChoices(),false));
        }
        for(uint64_t product_choice = 0; product_choice < this->productNumberOfChoices(); product_choice++) {
            auto[choice,memory] = this->product_choice_to_choice_memory[product_choice];
            for (auto const& label : this->quotient.getChoiceLabeling().getLabelsOfChoice(choice)) {
                product_labeling.addLabelToChoice(label,product_choice);
            }
        }
        return product_labeling;
    }

    template<typename ValueType>
        storm::models::sparse::StandardRewardModel<ValueType> QuotientPomdpManager<ValueType>::buildRewardModel(
            storm::models::sparse::StandardRewardModel<ValueType> const& reward_model
        ) {
            std::optional<std::vector<ValueType>> state_rewards, action_rewards;
            STORM_LOG_THROW(!reward_model.hasStateRewards(), storm::exceptions::NotSupportedException, "state rewards are currently not supported.");
            STORM_LOG_THROW(!reward_model.hasTransitionRewards(), storm::exceptions::NotSupportedException, "transition rewards are currently not supported.");
            
            action_rewards = std::vector<ValueType>();
            for(uint64_t product_choice = 0; product_choice < this->productNumberOfChoices(); product_choice++) {
                auto[choice,memory] = this->product_choice_to_choice_memory[product_choice];
                auto reward = reward_model.getStateActionReward(choice);
                action_rewards->push_back(reward);
            }
            return storm::models::sparse::StandardRewardModel<ValueType>(std::move(state_rewards), std::move(action_rewards));
        }


    template<typename ValueType>
    void QuotientPomdpManager<ValueType>::makeProductWithFsc(
        uint64_t num_nodes,
        std::vector<std::vector<uint64_t>> action_function,
        std::vector<std::vector<uint64_t>> update_function
    ) {

        this->buildStateSpace(num_nodes,action_function,update_function);
        storm::storage::sparse::ModelComponents<ValueType> components;
        components.stateLabeling = this->buildStateLabeling();
        components.transitionMatrix = this->buildTransitionMatrix();
        components.choiceLabeling = this->buildChoiceLabeling();
        for (auto const& reward_model : this->quotient.getRewardModels()) {
                auto new_reward_model = this->buildRewardModel(reward_model.second);
                components.rewardModels.emplace(reward_model.first, new_reward_model);
            }
        this->clearMemory();

        this->product = std::make_shared<storm::models::sparse::Mdp<ValueType>>(std::move(components));
    }

    template<typename ValueType>
    void QuotientPomdpManager<ValueType>::clearMemory() {
        this->state_memory_registered.clear();
        this->state_memory_to_product_state.clear();
        this->product_state_to_state_memory.clear();
        this->product_state_row_group_start.clear();
        this->product_choice_to_choice_memory.clear();
    }


    template class QuotientPomdpManager<double>;
}