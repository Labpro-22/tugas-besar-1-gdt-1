#pragma once
#include "exception/InvalidEntryInputException.hpp"

class InvalidDiceNumberException : public InvalidEntryInputException
{
private:
    int dice1Number;
    int dice2Number;

public:
    explicit InvalidDiceNumberException(int dice1Number, int dice2Number);
};