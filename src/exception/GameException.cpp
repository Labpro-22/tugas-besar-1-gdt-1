#include "exception/GameException.hpp"

GameException::GameException(int errorCode, const std::string &errorMessage)
    : errorCode(errorCode), errorMessage(errorMessage) {};

int GameException::getErrorCode() const
{
    return errorCode;
}

const char *GameException::what() const noexcept
{
    return errorMessage.c_str();
}
