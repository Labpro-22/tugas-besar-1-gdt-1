#include "models/CardAndDeck/ChanceCard.hpp"

ChanceCard::ChanceCard(ChanceType type)
    : ActionCard(initDescription(type)), type(type) {}

ChanceCard::~ChanceCard() {}

std::string ChanceCard::initDescription(ChanceType type) {
    switch (type) {
        case ChanceType::GO_TO_NEAREST_STATION:
            return "Pergi ke stasiun terdekat.";
        case ChanceType::MOVE_BACK_3:
            return "Mundur 3 petak.";
        case ChanceType::GO_TO_JAIL:
            return "Masuk Penjara.";
        default:
            return "";
    }
}

std::string ChanceCard::getCardName() const {
    return "ChanceCard";
}

CardCategory ChanceCard::getCategory() const {
    return CardCategory::CHANCE;
}

ChanceType ChanceCard::getType() const {
    return type;
}

Card* ChanceCard::clone() const {
    return new ChanceCard(type);
}