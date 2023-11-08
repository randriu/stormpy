#include "ItemTranslator.h"

namespace synthesis {

    ItemTranslator::ItemTranslator() {
        // left intentionally blank
    }

    ItemTranslator::ItemTranslator(uint64_t num_items) : num_items(num_items) {
        item_to_translation.resize(num_items, num_items);
    }

    uint64_t ItemTranslator::numTranslations() {
        return translation_to_item.size();
    }

    uint64_t ItemTranslator::translate(uint64_t item) {
        auto translation = item_to_translation[item];
        if(translation != num_items) {
            return translation;
        }
        auto new_translation = numTranslations();
        item_to_translation[item] = new_translation;
        translation_to_item.push_back(item);
        return new_translation;        
    }

    uint64_t ItemTranslator::retrieve(uint64_t translation) {
        return translation_to_item[translation];
    }

    std::vector<uint64_t> const& ItemTranslator::translationToItem() {
        return translation_to_item;
    }

    void ItemTranslator::clear() {
        num_items = 0;
        item_to_translation.clear();
        translation_to_item.clear();
    }

}