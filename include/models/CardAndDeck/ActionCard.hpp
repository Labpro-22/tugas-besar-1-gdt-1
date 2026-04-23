#ifndef ACTION_CARD_HPP
#define ACTION_CARD_HPP

#include "models/CardAndDeck/Card.hpp"

class ActionCard : public Card {
public:
    ActionCard(const std::string& description);
    virtual ~ActionCard();

    virtual CardCategory getCategory() const = 0;
    virtual Card* clone() const = 0;
};

#endif