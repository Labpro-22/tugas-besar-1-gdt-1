#include "models/BoardAndTiles/Board.hpp"
#include "models/BoardAndTiles/SpecialTile/GoTile.hpp"
#include "models/BoardAndTiles/SpecialTile/JailTile.hpp"
#include "models/BoardAndTiles/PropertyTile/RailroadTile.hpp"
#include "exception/InvalidEntryInput/InvalidTileException.hpp"
#include "exception/InvalidFile/InvalidConfigException.hpp"

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

Tile *Board::getTile(int index) const
{
    if (index < 1 || index > boardSize)
        throw InvalidTileException(std::to_string(index));

    return tiles[index - 1];
}

Tile *Board::getTile(const std::string &code) const
{
    auto it = tileByCode.find(code);

    if (it == tileByCode.end())
        throw InvalidTileException(code);

    return it->second;
}

int Board::getNextIndex(int currentIndex, int steps) const
{
    return (currentIndex - 1 + steps) % boardSize + 1;
}

bool Board::passesGo(int fromIndex, int steps) const
{
    if (goTile == nullptr)
        return false;

    int goIndex = goTile->getIndex();
    int toIndex = getNextIndex(fromIndex, steps);

    if (fromIndex == goIndex)
        return false;

    if (toIndex == goIndex)
        return false;

    if (toIndex > fromIndex)
        return goIndex > fromIndex && goIndex <= toIndex ? false : false;

    return fromIndex < goIndex || toIndex >= goIndex ? false : true;
}

GoTile *Board::getGoTile() const
{
    return goTile;
}

JailTile *Board::getJailTile() const
{
    return jailTile;
}

RailroadTile *Board::getNearestRailroad(int currentIndex) const
{
    RailroadTile *nearest = nullptr;
    int minSteps = boardSize + 1;

    for (Tile *tile : tiles)
    {
        RailroadTile *railroad = dynamic_cast<RailroadTile *>(tile);

        if (railroad == nullptr)
            continue;

        int steps = (railroad->getIndex() - currentIndex + boardSize) % boardSize;

        if (steps > 0 && steps < minSteps)
        {
            minSteps = steps;
            nearest = railroad;
        }
    }

    return nearest;
}

int Board::getSize() const
{
    return boardSize;
}

const std::vector<Tile *> &Board::getAllTiles() const
{
    return tiles;
}

void Board::validate() const
{
    if (boardSize < 20 || boardSize > 60)
    {
        throw InvalidConfigException("placeholder path", "Jumlah petak harus antara 20 dan 60, saat ini: " + std::to_string(boardSize));
    }

    if (goTile == nullptr)
    {
        throw InvalidConfigException("placeholder path", "Papan harus memiliki tepat 1 petak GO.");
    }

    if (jailTile == nullptr)
    {
        throw InvalidConfigException("placeholder path", "Papan harus memiliki tepat 1 petak GO.");
    }
}
