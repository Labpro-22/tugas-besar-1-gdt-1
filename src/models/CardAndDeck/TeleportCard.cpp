#include "models/CardAndDeck/TeleportCard.hpp"

TeleportCard::TeleportCard()
    : SkillCard("Pindah ke petak manapun di papan permainan.") {}

TeleportCard::~TeleportCard() {}

std::string TeleportCard::getCardName() const {
    return "TeleportCard";
}

Card* TeleportCard::clone() const {
    return new TeleportCard();
}