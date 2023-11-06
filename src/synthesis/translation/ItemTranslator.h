#pragma once

#include <cstdint>
#include <map>
#include <utility>
#include <vector>

namespace synthesis {

    template<typename K>
    class ItemTranslator {
    public:
        ItemTranslator();
        ItemTranslator(uint64_t num_items);
        uint64_t numTranslations();
        uint64_t translate(uint64_t item, K key);
        std::pair<uint64_t,K> retrieve(uint64_t translation);
        void clear();

        std::vector<uint64_t> translationToItem();

    private:
        uint64_t num_items;
        std::vector<std::map<K,uint64_t>> item_key_to_translation;
        std::vector<std::pair<uint64_t,K>> translation_to_item_key;
    };

}