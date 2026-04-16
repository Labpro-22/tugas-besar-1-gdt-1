#pragma once
#include "exception/PlayerTurnException.hpp"

class InvalidTurnException : public PlayerTurnException
{

public:
    explicit InvalidTurnException();
    const char *what() const noexcept override;
};