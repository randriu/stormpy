#pragma once

#include <cstdint>
#include <vector>

#include <storm/storage/BitVector.h>

namespace synthesis {

    class ItemTranslator {
    public:
        ItemTranslator();
        ItemTranslator(uint64_t num_items);
        uint64_t numTranslations();
        uint64_t translate(uint64_t item);
        uint64_t retrieve(uint64_t translation);
        void clear();

        std::vector<uint64_t> const& translationToItem();

    private:
        uint64_t num_items;
        std::vector<uint64_t> item_to_translation;
        std::vector<uint64_t> translation_to_item;
    };

}