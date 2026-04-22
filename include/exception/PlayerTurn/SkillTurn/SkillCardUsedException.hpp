#pragma once
#include "exception/PlayerTurn/SkillTurnException.hpp"

class SkillCardUsedException : public SkillTurnException
{
public:
    explicit SkillCardUsedException(Player *player, SkillCard *skillCard);
};