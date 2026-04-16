#pragma once
#include "exception/PlayerTurnException.hpp"
#include "models/CardAndDeck/SkillCard.hpp"

class SkillTurnException : public PlayerTurnException
{
protected:
    SkillCard *skillCard;

public:
    explicit SkillTurnException(int code, const std::string &msg, Player *player, SkillCard *skillCard);
};