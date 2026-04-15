#pragma once
#include "models/BoardAndTiles/Tile.hpp"

class ActionTile : public Tile
{
public:
    ActionTile(int index, const std::string &code, const std::string &name);

    TileCategory getCategory() const override;
};