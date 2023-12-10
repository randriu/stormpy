#include "Coloring.h"

#include <iostream>


namespace synthesis {

Family::Family() {
    // left intentionally blank
}

Family::Family(Family const& other) {
    hole_options = std::vector<std::vector<uint32_t>>(other.numHoles());
    hole_options_mask = std::vector<BitVector>(other.numHoles());
    for(uint32_t hole = 0; hole < numHoles(); ++hole) {
        hole_options[hole] = other.holeOptions(hole);
        hole_options_mask[hole] = other.holeOptionsMask(hole);
    }
}


uint32_t Family::numHoles() const {
    return hole_options.size();
}

void Family::addHole(uint32_t num_options) {
    hole_options_mask.push_back(BitVector(num_options,true));
    std::vector<uint32_t> options(num_options);
    for(uint32_t option=0; option<num_options; ++option) {
        options[option]=option;
    }
    hole_options.push_back(options);
}

std::vector<uint32_t> const& Family::holeOptions(uint32_t hole) const {
    return hole_options[hole];
}

BitVector const& Family::holeOptionsMask(uint32_t hole) const {
    return hole_options_mask[hole];
}


void Family::holeSetOptions(uint32_t hole, std::vector<uint32_t> const& options) {
    hole_options[hole] = options;
    hole_options_mask[hole].clear();
    for(auto option: options) {
        hole_options_mask[hole].set(option);
    }
}
void Family::holeSetOptions(uint32_t hole, BitVector const& options) {
    hole_options[hole].clear();
    for(auto option: options) {
        hole_options[hole].push_back(option);
    }
    hole_options_mask[hole] = options;
}





uint32_t Family::holeNumOptions(uint32_t hole) const {
    return hole_options[hole].size();
}

uint32_t Family::holeNumOptionsTotal(uint32_t hole) const {
    return hole_options_mask[hole].size();
}

bool Family::holeContains(uint32_t hole, uint32_t option) const {
    return hole_options_mask[hole][option];
}


bool Family::isSubsetOf(Family const& other) const {
    for(uint32_t hole = 0; hole < numHoles(); ++hole) {
        if(not hole_options_mask[hole].isSubsetOf(other.holeOptionsMask(hole))) {
            return false;
        }
    }
    return true;
}

bool Family::includesAssignment(std::vector<uint32_t> const& hole_to_option) const {
    for(uint32_t hole = 0; hole < numHoles(); ++hole) {
        if(not hole_options_mask[hole][hole_to_option[hole]]) {
            return false;
        }
    }
    return true;
}

bool Family::includesAssignment(std::map<uint32_t,uint32_t> const& hole_to_option) const {
    for(auto const& [hole,option]: hole_to_option) {
        if(not hole_options_mask[hole][option]) {
            return false;
        }
    }
    return true;
}

bool Family::includesAssignment(std::vector<std::pair<uint32_t,uint32_t>> const& hole_to_option) const {
    for(auto const& [hole,option]: hole_to_option) {
    if(not hole_options_mask[hole][option]) {
            return false;
        }
    }
    return true;   
}

std::vector<BitVector>::iterator Family::begin() {
    return hole_options_mask.begin();
}

std::vector<BitVector>::iterator Family::end() {
    return hole_options_mask.end();
}


void Family::setChoices(BitVector const& choices) {
    this->choices = BitVector(choices);
}

void Family::setChoices(BitVector && choices) {
    this->choices = choices;
}

BitVector const& Family::getChoices() const {
    return choices;
}

}