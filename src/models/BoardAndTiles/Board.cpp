#include "models/BoardAndTiles/Board.hpp"
#include "models/BoardAndTiles/SpecialTile/GoTile.hpp"
#include "models/BoardAndTiles/SpecialTile/JailTile.hpp"
#include "models/BoardAndTiles/PropertyTile.hpp"
#include "models/BoardAndTiles/PropertyTile/RailroadTile.hpp"
#include "models/Property/StreetProperty.hpp"
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

    if (tile->getKind() == TileKind::GO)
        goTile = static_cast<GoTile *>(tile);

    if (tile->getKind() == TileKind::JAIL)
        jailTile = static_cast<JailTile *>(tile);
}

Tile *Board::getTile(int index) const
{
    if (index < 0 || index >= boardSize)
        throw InvalidTileException(std::to_string(index));

    return tiles[index];
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
    return (currentIndex + steps) % boardSize;
}

bool Board::passesGo(int fromIndex, int steps) const
{
    if (goTile == nullptr) return false;
    if (steps <= 0) return false;

    int boardSize = this->boardSize;
    int goIndex = goTile->getIndex();
    int toIndex = (fromIndex + steps) % boardSize;

    if (fromIndex < toIndex)
    {
        return goIndex > fromIndex && goIndex <= toIndex;
    }
    else
    {
        return goIndex > fromIndex || goIndex <= toIndex;
    }
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
        if (tile->getKind() != TileKind::RAILROAD)
            continue;

        RailroadTile *railroad = static_cast<RailroadTile *>(tile);

        int steps = (railroad->getIndex() - currentIndex + boardSize) % boardSize;

        if (steps > 0 && steps < minSteps)
        {
            minSteps = steps;
            nearest = railroad;
        }
    }

    return nearest;
}

std::vector<StreetProperty *> Board::getStreetGroup(const std::string &colorGroup) const
{
    std::vector<StreetProperty *> streets;

    for (Tile *tile : tiles)
    {
        if (tile->getCategory() != TileCategory::PROPERTY)
            continue;

        auto *propertyTile = static_cast<PropertyTile *>(tile);

        Property *property = propertyTile->getProperty();
        if (property == nullptr || !property->isStreet())
            continue;

        auto *street = static_cast<StreetProperty *>(property);

        if (street->getColorGroup() == colorGroup)
        {
            streets.push_back(street);
        }
    }

    return streets;
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
