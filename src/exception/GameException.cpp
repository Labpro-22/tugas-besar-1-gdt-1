#include "exception/GameException.hpp"

GameException::GameException(int errorCode, const std::string &errorMessage)
    : errorCode(errorCode), errorMessage(errorMessage) {};