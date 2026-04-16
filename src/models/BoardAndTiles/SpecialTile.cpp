#include "models/BoardAndTiles/SpecialTile.hpp"

SpecialTile::SpecialTile(int index, const std::string &code, const std::string &name)
    : Tile(index, code, name, TileColor::DEFAULT) {}

TileCategory SpecialTile::getCategory() const
{
    return TileCategory::SPECIAL;
}