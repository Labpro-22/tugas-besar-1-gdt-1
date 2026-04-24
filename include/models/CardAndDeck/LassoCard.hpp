#ifndef LASSO_CARD_HPP
#define LASSO_CARD_HPP

#include "models/CardAndDeck/SkillCard.hpp"

class LassoCard : public SkillCard {
public:
    LassoCard();
    ~LassoCard();

    std::string getCardName() const override;
    SkillCardKind getKind() const override;
    Card* clone() const override;
};

#endif
