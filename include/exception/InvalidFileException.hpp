#pragma once
#include "exception/GameException.hpp"

class InvalidFileException : public GameException
{
protected:
    std::string filePath;

public:
    explicit InvalidFileException(const std::string &filePath);
};