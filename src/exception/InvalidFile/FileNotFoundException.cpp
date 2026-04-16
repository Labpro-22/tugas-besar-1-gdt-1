#include "exception/InvalidFile/FileNotFoundException.hpp"

FileNotFoundException::FileNotFoundException(const std::string &filePath)
    : InvalidFileException(310, "File Not Found: ", filePath) {};

const char *FileNotFoundException::what() const noexcept
{
    return errorMessage.c_str();
};