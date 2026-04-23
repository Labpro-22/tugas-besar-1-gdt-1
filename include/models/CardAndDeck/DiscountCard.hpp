#ifndef DISCOUNT_CARD_HPP
#define DISCOUNT_CARD_HPP

#include "models/CardAndDeck/SkillCard.hpp"

class DiscountCard : public SkillCard {
private:
    int discountPercent;
    int duration;

public:
    DiscountCard(int discountPercent);
    ~DiscountCard();

    std::string getCardName() const override;
    Card* clone() const override;

    int getDiscount() const;
    int getDuration() const;
    void decrementDuration();
};

#endif