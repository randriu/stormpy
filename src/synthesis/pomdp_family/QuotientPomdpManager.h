#pragma once

#include "storm/models/sparse/Model.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/storage/BitVector.h"

namespace synthesis {

    template<typename ValueType>
    class QuotientPomdpManager {

    public:

        QuotientPomdpManager(
            storm::models::sparse::Model<ValueType> const& quotient,
            std::vector<uint32_t> state_to_obs_class,
            uint64_t num_actions,
            std::vector<uint64_t> choice_to_action
        );

        /**
         * Create a product of the quotient POMDP and the given FSC.
         * @param num_nodes number of nodes of the FSC
         * @param action_function for each node in the FSC and for each observation class, an index of the choice
         * @param action_function for each node in the FSC and for each observation class, a memory update
         */
        void makeProductWithFsc(
            uint64_t num_nodes,
            std::vector<std::vector<uint64_t>> action_function,
            std::vector<std::vector<uint64_t>> update_function
        );

        /** The constructed product with an FSC. */
        std::shared_ptr<storm::models::sparse::Mdp<ValueType>> product;
        /** For each choice of the product MDP, its original choice. */
        std::vector<uint64_t> product_choice_to_choice;

        

    private:
        
        /** The quotient model. */
        storm::models::sparse::Model<ValueType> const& quotient;
        /** For each state of the quotient, its observation class. */
        std::vector<uint32_t> state_to_obs_class;
        /** Overall number of actions. */
        uint64_t num_actions;
        /** For each choice of the quotient, the corresponding action. */
        std::vector<uint64_t> choice_to_action;
        
        /** For each state-action pair, a list of choices that implement this action. */
        std::vector<std::vector<std::set<uint64_t>>> state_action_choices;

        /** For each product state, its state-memory value. */
        std::vector<std::pair<uint64_t,uint64_t>> product_state_to_state_memory;
        /** For aeach product choice, its choice-memory value. */
        std::vector<std::pair<uint64_t,uint64_t>> product_choice_to_choice_memory;
        /** For each state-memory pair, whether it has been registered. */
        std::vector<storm::storage::BitVector> state_memory_registered;
        /** For each state-memory pair, its corresponding product state. */
        std::vector<std::vector<uint64_t>> state_memory_to_product_state;
        /** For each product state, its first choice. */
        std::vector<uint64_t> product_state_row_group_start;
        
        /** Number of states in the product. */
        uint64_t productNumberOfStates();
        /** Number of states in the product. */
        uint64_t productNumberOfChoices();

        
        /** Given a state-memory pair, retreive the corresponding product state or create a new one. */
        uint64_t mapStateMemory(uint64_t state, uint64_t memory);
        
        void buildStateSpace(
            uint64_t num_nodes,
            std::vector<std::vector<uint64_t>> action_function,
            std::vector<std::vector<uint64_t>> update_function
        );
        storm::models::sparse::StateLabeling buildStateLabeling();
        storm::storage::SparseMatrix<ValueType> buildTransitionMatrix();
        storm::models::sparse::ChoiceLabeling buildChoiceLabeling();
        storm::models::sparse::StandardRewardModel<ValueType> buildRewardModel(
            storm::models::sparse::StandardRewardModel<ValueType> const& reward_model
        );

        void clearMemory();

    };
}