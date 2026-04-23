#include "models/CardAndDeck/LassoCard.hpp"

LassoCard::LassoCard()
    : SkillCard("Tarik pemain lawan di depanmu ke petakmu.") {}

LassoCard::~LassoCard() {}

std::string LassoCard::getCardName() const {
    return "LassoCard";
}

Card* LassoCard::clone() const {
    return new LassoCard();
}