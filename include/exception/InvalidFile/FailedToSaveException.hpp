#pragma once
#include "exception/InvalidFileException.hpp"

class FailedToSaveException : public InvalidFileException
{
public:
    explicit FailedToSaveException(const std::string &filePath);
};