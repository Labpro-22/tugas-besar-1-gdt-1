#ifndef SKILL_CARD_HPP
#define SKILL_CARD_HPP

#include "models/CardAndDeck/Card.hpp"

enum class SkillCardKind {
    MOVE,
    TELEPORT,
    DISCOUNT,
    SHIELD,
    LASSO,
    DEMOLITION,
    JAIL_FREE
};

class SkillCard : public Card {
protected:
    int remainingUses;

public:
    SkillCard(const std::string& description, int uses = 1);
    virtual ~SkillCard();

    CardCategory getCategory() const override;
    bool isUsable() const;
    virtual bool isJailFreeCard() const;
    virtual SkillCardKind getKind() const = 0;
    virtual int getPrimaryValue() const;
    virtual int getDurationValue() const;

    virtual std::string getCardName() const = 0;
    virtual Card* clone() const = 0;
};

#endif
