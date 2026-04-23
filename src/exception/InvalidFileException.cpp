#include "exception/InvalidFileException.hpp"

InvalidFileException::InvalidFileException(int code, const std::string &msg, const std::string &filePath)
    : filePath(filePath), GameException(code, msg) {};