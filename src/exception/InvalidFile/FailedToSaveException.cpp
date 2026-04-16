#include "exception/InvalidFile/FailedToSaveException.hpp"

FailedToSaveException::FailedToSaveException(const std::string &filePath)
    : InvalidFileException(330, "Failed to Save: ", filePath) {};

const char *FailedToSaveException::what() const noexcept
{
    return errorMessage.c_str();
};