#include "exception/InvalidFile/UnreadableFileException.hpp"

UnreadableFileException::UnreadableFileException(const std::string &filePath)
    : InvalidFileException(320, "Unreadable File: ", filePath)
{
    errorMessage += "Lokasi: " + filePath +
                    " (File berhasil ditemukan, tetapi formatnya rusak atau tidak dapat diparsing)";
}