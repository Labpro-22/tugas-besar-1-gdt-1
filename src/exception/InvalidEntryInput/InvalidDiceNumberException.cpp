#include "exception/InvalidEntryInput/InvalidDiceNumberException.hpp"

InvalidDiceNumberException::InvalidDiceNumberException(int dice1Number, int dice2Number)
    : InvalidEntryInputException(210, "Dice Number Invalid: "), dice1Number(dice1Number), dice2Number(dice2Number)
{
    errorMessage += "Dadu 1 (" + std::to_string(dice1Number) +
                    "), Dadu 2 (" + std::to_string(dice2Number) +
                    ") - Angka dadu harus berada di rentang yang valid.";
}