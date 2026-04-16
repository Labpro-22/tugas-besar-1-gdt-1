#include "DemolitionCard.hpp"

DemolitionCard::DemolitionCard()
    : SkillCard("Hancurkan satu bangunan milik pemain lawan.") {}

DemolitionCard::~DemolitionCard() {}

std::string DemolitionCard::getCardName() const {
    return "DemolitionCard";
}

Card* DemolitionCard::clone() const {
    return new DemolitionCard();
}