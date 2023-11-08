#include "ItemKeyTranslator.h"

namespace synthesis {

    template<typename K>
    ItemKeyTranslator<K>::ItemKeyTranslator() {
        // left intentionally blank
    }

    template<typename K>
    ItemKeyTranslator<K>::ItemKeyTranslator(uint64_t num_items) {
        item_key_to_translation.resize(num_items);
    }

    template<typename K>
    uint64_t ItemKeyTranslator<K>::numTranslations() {
        return translation_to_item_key.size();
    }

    template<typename K>
    uint64_t ItemKeyTranslator<K>::translate(uint64_t item, K key) {
        auto new_translation = numTranslations();
        auto const& result = item_key_to_translation[item].try_emplace(key,new_translation);
        if(result.second) {
            // new item
            translation_to_item_key.push_back(std::make_pair(item,key));
        }
        return (*result.first).second;
    }

    template<typename K>
    std::pair<uint64_t,K> ItemKeyTranslator<K>::retrieve(uint64_t translation) {
        return translation_to_item_key[translation];
    }

    template<typename K>
    std::vector<uint64_t> ItemKeyTranslator<K>::translationToItem() {
        std::vector<uint64_t> translation_to_item(numTranslations());
        for(uint64_t translation = 0; translation<numTranslations(); translation++) {
            translation_to_item[translation] = translation_to_item_key[translation].first;
        }
        return translation_to_item;
    }

    template<typename K>
    void ItemKeyTranslator<K>::clear() {
        item_key_to_translation.clear();
        translation_to_item_key.clear();
    }

    template class ItemKeyTranslator<uint64_t>;
    template class ItemKeyTranslator<std::pair<uint64_t,uint64_t>>;
}