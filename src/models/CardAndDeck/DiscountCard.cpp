#include "models/CardAndDeck/DiscountCard.hpp"

DiscountCard::DiscountCard(int discountPercent)
    : SkillCard("Diskon " + std::to_string(discountPercent) + "% untuk pembelian properti."),
      discountPercent(discountPercent),
      duration(1) {}

DiscountCard::~DiscountCard() {}

std::string DiscountCard::getCardName() const {
    return "DiscountCard";
}

SkillCardKind DiscountCard::getKind() const {
    return SkillCardKind::DISCOUNT;
}

Card* DiscountCard::clone() const {
    return new DiscountCard(discountPercent);
}

int DiscountCard::getDiscount() const {
    return discountPercent;
}

int DiscountCard::getDuration() const {
    return duration;
}

void DiscountCard::decrementDuration() {
    if (duration > 0) duration--;
}

int DiscountCard::getPrimaryValue() const {
    return discountPercent;
}

int DiscountCard::getDurationValue() const {
    return duration;
}
