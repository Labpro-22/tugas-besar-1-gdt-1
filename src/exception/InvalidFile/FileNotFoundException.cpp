#include "exception/InvalidFile/FileNotFoundException.hpp"

FileNotFoundException::FileNotFoundException(const std::string &filePath)
    : InvalidFileException(310, "File Not Found: ", filePath)
{
    errorMessage += "Lokasi: " + filePath +
                    " (Pastikan nama file dan direktori sudah benar)";
}