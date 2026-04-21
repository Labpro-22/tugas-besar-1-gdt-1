#include "exception/InvalidEntryInput/InvalidTileException.hpp"

InvalidTileException::InvalidTileException(const std::string &tileCode)
    : InvalidEntryInputException(220, "Invalid Tile: "), tileCode(tileCode)
{
    errorMessage += "Kode Tile yang dicari: '" + tileCode +
                    "' (Tile ini tidak ditemukan di papan permainan)";
}