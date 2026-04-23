#pragma once
#include "models/BoardAndTiles/Tile.hpp"

class Property;

class PropertyTile : public Tile
{
protected:
    Property *property;

public:
    PropertyTile(int index, const std::string &code, const std::string &name, TileColor color, Property *property);

    Property *getProperty() const;
    TileCategory getCategory() const override;
    std::string getDisplayLabel() const override;
};