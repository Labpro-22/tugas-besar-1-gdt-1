#include "exception/InvalidEntryInput/InvalidTileException.hpp"

InvalidTileException::InvalidTileException(const std::string &tileCode)
    : InvalidEntryInputException(210, "Dice Number Invalid: "), tileCode(tileCode) {};

const char *InvalidTileException::what() const noexcept
{
    return errorMessage.c_str();
};