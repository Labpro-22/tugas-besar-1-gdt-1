#pragma once
#include "models/BoardAndTiles/Tile.hpp"

class SpecialTile : public Tile
{
public:
    SpecialTile(int index, const std::string &code, const std::string &name);

    TileCategory getCategory() const override;
};