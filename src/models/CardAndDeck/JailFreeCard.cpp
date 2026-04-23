#include "models/CardAndDeck/JailFreeCard.hpp"

JailFreeCard::JailFreeCard()
    : SkillCard("Bebas dari Penjara.") {}

JailFreeCard::~JailFreeCard() {}

bool JailFreeCard::isJailFreeCard() const {
    return true;
}

std::string JailFreeCard::getCardName() const {
    return "JailFreeCard";
}

Card* JailFreeCard::clone() const {
    return new JailFreeCard();
}
