#pragma once
#include "exception/PlayerTurnException.hpp"
#include "models/CardAndDeck/SkillCard.hpp"

class SkillTurnException : public PlayerTurnException
{
protected:
    SkillCard *skillCard;

public:
    explicit SkillTurnException(const SkillCard &skillCard);
};