#include "models/BoardAndTiles/Board.hpp"

Board::Board() : boardSize(0), goTile(nullptr), jailTile(nullptr) {}

Board::~Board()
{
    for (Tile *tile : tiles)
        delete tile;
    tiles.clear();
}

void Board::addTile(Tile *tile)
{
    tiles.push_back(tile);
    tileByCode[tile->getCode()] = tile;
    boardSize = static_cast<int>(tiles.size());

    if (GoTile *go = dynamic_cast<GoTile *>(tile))
        goTile = go;

    if (JailTile *jail = dynamic_cast<JailTile *>(tile))
        jailTile = jail;
}