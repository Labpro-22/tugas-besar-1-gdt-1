#include "exception/InvalidEntryInput/InvalidDiceNumberException.hpp"

InvalidDiceNumberException::InvalidDiceNumberException(int dice1Number, int dice2Number)
    : InvalidEntryInputException(210, "Dice Number Invalid: "), dice1Number(dice1Number), dice2Number(dice2Number) {};

const char *InvalidDiceNumberException::what() const noexcept
{
    return errorMessage.c_str();
};