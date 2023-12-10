#pragma once

#include "src/synthesis/quotient/Family.h"

#include <storm/storage/BitVector.h>

#include <cstdint>
#include <vector>

namespace synthesis {

using BitVector = storm::storage::BitVector;

class Coloring {
public:
    
    Coloring(
        Family const& family, std::vector<uint64_t> const& row_groups,
        std::vector<std::vector<std::pair<uint32_t,uint32_t>>> choice_to_assignment
    );

    std::vector<std::vector<std::pair<uint32_t,uint32_t>>> const& getChoiceToAssignment() const;
    std::vector<BitVector> const& getStateToHoles() const;
    BitVector const& getUncoloredChoices() const;

    BitVector selectCompatibleChoices(Family const& subfamily) const;
    
    std::vector<BitVector> collectHoleOptionsMask(BitVector const& choices) const;
    
    std::vector<std::vector<uint32_t>> collectHoleOptions(BitVector const& choices) const;
    
protected:

    /** Reference to the unrefined family. */
    Family family;
    /** For each choice, a list of hole-option pairs (colors). */
    const std::vector<std::vector<std::pair<uint32_t,uint32_t>>> choice_to_assignment;

    /** Number of choices in the quotient. */
    const uint64_t numChoices() const;
    
    /** For each state, identification of holes associated with its choices. */
    std::vector<BitVector> choice_to_holes;
    /** For each state, identification of holes associated with its choices. */
    std::vector<BitVector> state_to_holes;
    /** Whether all states have at most one hole associated with its choices. */
    bool is_simple;

    /** Choices not labeled by any hole. */
    BitVector uncolored_choices;
    /** Choices labeled by some hole. */
    BitVector colored_choices;
}; 

}