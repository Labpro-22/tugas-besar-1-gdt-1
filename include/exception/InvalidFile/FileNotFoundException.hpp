#pragma once
#include "exception/InvalidFileException.hpp"

class FileNotFoundException : public InvalidFileException
{
public:
    explicit FileNotFoundException(const std::string &filePath);
};