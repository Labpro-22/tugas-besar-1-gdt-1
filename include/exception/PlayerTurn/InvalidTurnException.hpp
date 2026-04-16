#pragma once
#include "exception/PlayerTurnException.hpp"

class InvalidTurnException : public PlayerTurnException
{

public:
    explicit InvalidTurnException(Player *player);
    const char *what() const noexcept override;
};