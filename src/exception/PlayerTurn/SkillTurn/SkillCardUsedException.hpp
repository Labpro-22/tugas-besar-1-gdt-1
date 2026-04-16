#pragma once
#include "exception/PlayerTurn/SkillTurnException.hpp"

class SkillCardUsedException : public SkillTurnException
{
public:
    explicit SkillCardUsedException();
    const char *what() const noexcept override;
};