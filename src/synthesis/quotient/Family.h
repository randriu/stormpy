#pragma once

#include <storm/storage/BitVector.h>

#include <cstdint>
#include <vector>
#include <map>

namespace synthesis {

using BitVector = storm::storage::BitVector;

class Family {
public:
    
    Family();
    Family(Family const& other);

    uint32_t numHoles() const;
    void addHole(uint32_t num_options);
    
    std::vector<uint32_t> const& holeOptions(uint32_t hole) const;
    BitVector const& holeOptionsMask(uint32_t hole) const;

    void holeSetOptions(uint32_t hole, std::vector<uint32_t> const& options);
    void holeSetOptions(uint32_t hole, BitVector const& options);
    // void holeSetOptions(uint32_t hole, BitVector&& options);

    uint32_t holeNumOptions(uint32_t hole) const;
    uint32_t holeNumOptionsTotal(uint32_t hole) const;
    bool holeContains(uint32_t hole, uint32_t option) const;

    bool includesAssignment(std::vector<uint32_t> const& hole_to_option) const;
    bool includesAssignment(std::map<uint32_t,uint32_t> const& hole_to_option) const;
    bool includesAssignment(std::vector<std::pair<uint32_t,uint32_t>> const& hole_to_option) const;
    
    bool isSubsetOf(Family const& other) const;
    // uint64_t size();

    // iterator over hole options
    std::vector<BitVector>::iterator begin();
    std::vector<BitVector>::iterator end();

    // choice operations
    void setChoices(BitVector const& choices);
    void setChoices(BitVector&& choices);
    BitVector const& getChoices() const;
    
    
protected:
    /** For each hole, a list of available options. */
    std::vector<std::vector<uint32_t>> hole_options;
    /** For each hole, a mastk of available options. */
    std::vector<BitVector> hole_options_mask;
    
    
    /** Whether choices have been set for this family. */
    bool choices_set = false;
    /** Bitvector of choices relevant to this family. */
    BitVector choices;
};

}