#include "models/BoardAndTiles/Tile.hpp"

Tile::Tile(int index, const std::string &code, const std::string &name, TileColor color)
    : index(index), code(code), name(name), color(color) {}

int Tile::getIndex() const { return index; }

const std::string &Tile::getCode() const { return code; }

const std::string &Tile::getName() const { return name; }

TileColor Tile::getColor() const { return color; }

void Tile::onPassed(Player &player, Game &game)
{
    // default: no-op
}

std::string Tile::getDisplayLabel() const
{
    return "[" + code + "]";
}