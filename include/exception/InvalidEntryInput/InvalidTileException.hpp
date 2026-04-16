#pragma once
#include "exception/InvalidEntryInputException.hpp"

class InvalidTileException : public InvalidEntryInputException
{
private:
    std::string tileCode;

public:
    explicit InvalidTileException(const std::string &tileCode);
    const char *what() const noexcept override;
};