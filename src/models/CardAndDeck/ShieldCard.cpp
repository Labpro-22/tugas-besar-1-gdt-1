#include "models/CardAndDeck/ShieldCard.hpp"

ShieldCard::ShieldCard()
    : SkillCard("Kebal terhadap tagihan atau sanksi selama 1 giliran."),
      duration(1) {}

ShieldCard::~ShieldCard() {}

std::string ShieldCard::getCardName() const {
    return "ShieldCard";
}

SkillCardKind ShieldCard::getKind() const {
    return SkillCardKind::SHIELD;
}

Card* ShieldCard::clone() const {
    return new ShieldCard();
}

int ShieldCard::getDuration() const {
    return duration;
}

void ShieldCard::decrementDuration() {
    if (duration > 0) duration--;
}

int ShieldCard::getDurationValue() const {
    return duration;
}
