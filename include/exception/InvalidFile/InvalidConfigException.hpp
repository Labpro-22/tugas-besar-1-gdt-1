#pragma once
#include "exception/InvalidFileException.hpp"

class InvalidConfigException : public InvalidFileException
{
public:
    explicit InvalidConfigException(const std::string &filePath, const std::string &reason);
};
