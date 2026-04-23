#include "exception/InvalidFile/FailedToSaveException.hpp"

FailedToSaveException::FailedToSaveException(const std::string &filePath)
    : InvalidFileException(330, "Failed to Save: ", filePath)
{
    errorMessage += "Lokasi: " + filePath +
                    " (Terjadi kesalahan sistem saat mencoba menulis data ke file tersebut)";
}