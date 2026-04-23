#ifndef MOVE_CARD_HPP
#define MOVE_CARD_HPP

#include "models/CardAndDeck/SkillCard.hpp"

class MoveCard : public SkillCard {
private:
    int steps;

public:
    MoveCard(int steps);
    ~MoveCard();

    std::string getCardName() const override;
    Card* clone() const override;

    int getSteps() const;
};

#endif