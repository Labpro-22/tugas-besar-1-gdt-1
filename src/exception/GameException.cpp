#include "exception/GameException.hpp"

GameException::GameException(int errorCode, const std::string &errorMessage)
    : errorCode(errorCode), errorMessage(errorMessage) {};

const char *GameException::what() const noexcept
{
    return errorMessage.c_str();
}
