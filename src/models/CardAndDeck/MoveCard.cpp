#include "models/CardAndDeck/MoveCard.hpp"

MoveCard::MoveCard(int steps)
    : SkillCard("Maju " + std::to_string(steps) + " petak."), steps(steps) {}

MoveCard::~MoveCard() {}

std::string MoveCard::getCardName() const {
    return "MoveCard";
}

SkillCardKind MoveCard::getKind() const {
    return SkillCardKind::MOVE;
}

Card* MoveCard::clone() const {
    return new MoveCard(steps);
}

int MoveCard::getSteps() const {
    return steps;
}

int MoveCard::getPrimaryValue() const {
    return steps;
}
