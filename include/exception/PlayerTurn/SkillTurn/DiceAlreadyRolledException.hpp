#pragma once
#include "exception/PlayerTurn/SkillTurnException.hpp"

class DiceAlreadyRolledException : public SkillTurnException
{
public:
    explicit DiceAlreadyRolledException(Player *player, SkillCard *skillCard);
};