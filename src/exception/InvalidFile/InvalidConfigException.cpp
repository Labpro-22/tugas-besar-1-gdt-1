#include "exception/InvalidFile/InvalidConfigException.hpp"

InvalidConfigException::InvalidConfigException(const std::string &filePath, const std::string &reason)
    : InvalidFileException(340, "Invalid Config (" + reason + "): ", filePath)
{
    errorMessage += "Lokasi: " + filePath +
                    " | Detail Kesalahan: " + reason;
}