#pragma once
#include "exception/GameException.hpp"

class InvalidFileException : public GameException
{
protected:
    std::string filePath;

public:
    explicit InvalidFileException(int code, const std::string &msg, const std::string &filePath);
};