#pragma once
#include "exception/PlayerTurn/SkillTurnException.hpp"

class DiceAlreadyRolledException : public SkillTurnException
{
public:
    explicit DiceAlreadyRolledException();
    const char *what() const noexcept override;
};