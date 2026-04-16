#include "exception/PlayerTurn/InvalidTurnException.hpp"

InvalidTurnException::InvalidTurnException(Player *player)
    : PlayerTurnException(130, "Error Turn: ", player) {};

const char *InvalidTurnException::what() const noexcept
{
    return errorMessage.c_str();
}
