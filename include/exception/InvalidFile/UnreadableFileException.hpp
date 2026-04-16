#pragma once
#include "exception/InvalidFileException.hpp"

class UnreadableFileException : public InvalidFileException
{
public:
    explicit UnreadableFileException(const std::string &filePath);
    const char *what() const noexcept override;
};