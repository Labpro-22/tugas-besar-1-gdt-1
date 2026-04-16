#ifndef DEMOLITION_CARD_HPP
#define DEMOLITION_CARD_HPP

#include "SkillCard.hpp"

class DemolitionCard : public SkillCard {
public:
    DemolitionCard();
    ~DemolitionCard();

    std::string getCardName() const override;
    Card* clone() const override;
};

#endif