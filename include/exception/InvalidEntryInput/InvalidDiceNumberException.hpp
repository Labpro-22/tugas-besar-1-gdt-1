#pragma once
#include "exception/InvalidEntryInputException.hpp"

class InvalidDiceNumberException : public InvalidEntryInputException
{
private:
    int dice1Number;
    int dice2Number;

public:
    explicit InvalidDiceNumberException(const int &dice1Number, const int &dice2Number);
    const char *what() const noexcept override;
};