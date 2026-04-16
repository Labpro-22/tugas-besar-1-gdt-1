#include "models/BoardAndTiles/ActionTile.hpp"

ActionTile::ActionTile(int index, const std::string &code, const std::string &name)
    : Tile(index, code, name, TileColor::DEFAULT) {}

TileCategory ActionTile::getCategory() const
{
    return TileCategory::ACTION;
}