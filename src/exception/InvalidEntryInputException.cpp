#include "exception/InvalidEntryInputException.hpp"

InvalidEntryInputException::InvalidEntryInputException(int code, const std::string &msg)
    : GameException(code, msg) {};