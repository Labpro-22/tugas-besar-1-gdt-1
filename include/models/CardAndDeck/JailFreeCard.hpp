#ifndef JAIL_FREE_CARD_HPP
#define JAIL_FREE_CARD_HPP

#include "models/CardAndDeck/SkillCard.hpp"

class JailFreeCard : public SkillCard {
public:
    JailFreeCard();
    ~JailFreeCard() override;

    bool isJailFreeCard() const override;
    std::string getCardName() const override;
    Card* clone() const override;
};

#endif
