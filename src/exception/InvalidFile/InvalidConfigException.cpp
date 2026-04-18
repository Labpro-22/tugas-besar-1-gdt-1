#include "exception/InvalidFile/InvalidConfigException.hpp"

InvalidConfigException::InvalidConfigException(const std::string &filePath, const std::string &reason)
    : InvalidFileException(320, "Invalid Config (" + reason + "): ", filePath) {};

const char *InvalidConfigException::what() const noexcept
{
    return errorMessage.c_str();
};
