#ifndef TELEPORT_CARD_HPP
#define TELEPORT_CARD_HPP

#include "models/CardAndDeck/SkillCard.hpp"

class TeleportCard : public SkillCard {
public:
    TeleportCard();
    ~TeleportCard();

    std::string getCardName() const override;
    Card* clone() const override;
};

#endif