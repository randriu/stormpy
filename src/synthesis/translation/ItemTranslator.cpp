#include "ItemTranslator.h"

namespace synthesis {

    template<typename K>
    ItemTranslator<K>::ItemTranslator() {
        // left intentionally blank
    }

    template<typename K>
    ItemTranslator<K>::ItemTranslator(uint64_t num_items) : num_items(num_items) {
        item_key_to_translation.resize(num_items);
    }

    template<typename K>
    uint64_t ItemTranslator<K>::numTranslations() {
        return translation_to_item_key.size();
    }

    template<typename K>
    uint64_t ItemTranslator<K>::translate(uint64_t item, K key) {
        auto new_translation = numTranslations();
        auto const& result = item_key_to_translation[item].try_emplace(key,new_translation);
        if(result.second) {
            // new item
            translation_to_item_key.push_back(std::make_pair(item,key));
        }
        return (*result.first).second;
    }

    template<typename K>
    std::pair<uint64_t,K> ItemTranslator<K>::retrieve(uint64_t translation) {
        return translation_to_item_key[translation];
    }

    template<typename K>
    std::vector<uint64_t> ItemTranslator<K>::translationToItem() {
        std::vector<uint64_t> translation_to_item(numTranslations());
        for(uint64_t translation = 0; translation<numTranslations(); translation++) {
            translation_to_item[translation] = translation_to_item_key[translation].first;
        }
        return translation_to_item;
    }

    template<typename K>
    void ItemTranslator<K>::clear() {
        num_items = 0;
        item_key_to_translation.clear();
        translation_to_item_key.clear();
    }

    template class ItemTranslator<uint64_t>;
    template class ItemTranslator<std::pair<uint64_t,uint64_t>>;
}