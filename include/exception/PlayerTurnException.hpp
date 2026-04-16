#pragma once
#include "exception/GameException.hpp"
#include "models/Player/Player.hpp"

class PlayerTurnException : public GameException
{
protected:
    Player *player;

public:
    explicit PlayerTurnException(const Player &player);
};