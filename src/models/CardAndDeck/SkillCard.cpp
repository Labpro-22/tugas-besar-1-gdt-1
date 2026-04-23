#include "models/CardAndDeck/SkillCard.hpp"

SkillCard::SkillCard(const std::string& description, int uses)
    : Card(description), remainingUses(uses) {}

SkillCard::~SkillCard() {}

CardCategory SkillCard::getCategory() const {
    return CardCategory::SKILL;
}

bool SkillCard::isUsable() const {
    return remainingUses > 0;
}

bool SkillCard::isJailFreeCard() const {
    return false;
}
