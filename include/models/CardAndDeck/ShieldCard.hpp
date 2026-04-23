#ifndef SHIELD_CARD_HPP
#define SHIELD_CARD_HPP

#include "models/CardAndDeck/SkillCard.hpp"

class ShieldCard : public SkillCard {
private:
    int duration;

public:
    ShieldCard();
    ~ShieldCard();

    std::string getCardName() const override;
    Card* clone() const override;

    int getDuration() const;
    void decrementDuration();
};

#endif