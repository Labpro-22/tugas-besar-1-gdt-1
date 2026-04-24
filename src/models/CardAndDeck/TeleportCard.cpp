#include "models/CardAndDeck/TeleportCard.hpp"

TeleportCard::TeleportCard()
    : SkillCard("Pindah ke petak manapun di papan permainan.") {}

TeleportCard::~TeleportCard() {}

std::string TeleportCard::getCardName() const {
    return "TeleportCard";
}

SkillCardKind TeleportCard::getKind() const {
    return SkillCardKind::TELEPORT;
}

Card* TeleportCard::clone() const {
    return new TeleportCard();
}
