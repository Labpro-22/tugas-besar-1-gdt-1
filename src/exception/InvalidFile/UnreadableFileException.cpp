#include "exception/InvalidFile/UnreadableFileException.hpp"

UnreadableFileException::UnreadableFileException(const std::string &filePath)
    : InvalidFileException(310, "Unreadable File: ", filePath) {};

const char *UnreadableFileException::what() const noexcept
{
    return errorMessage.c_str();
};