#pragma once
#include "exception/GameException.hpp"

class InvalidEntryInputException : public GameException
{
public:
    explicit InvalidEntryInputException(int code, const std::string &msg);
};