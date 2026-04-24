#include "models/CardAndDeck/DemolitionCard.hpp"

DemolitionCard::DemolitionCard()
    : SkillCard("Hancurkan satu bangunan milik pemain lawan.") {}

DemolitionCard::~DemolitionCard() {}

std::string DemolitionCard::getCardName() const {
    return "DemolitionCard";
}

SkillCardKind DemolitionCard::getKind() const {
    return SkillCardKind::DEMOLITION;
}

Card* DemolitionCard::clone() const {
    return new DemolitionCard();
}
