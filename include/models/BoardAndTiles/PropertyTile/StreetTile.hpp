// StreetTile.h
#pragma once
#include "models/BoardAndTiles/PropertyTile.hpp"

class StreetProperty;

class StreetTile : public PropertyTile
{
public:
    StreetTile(int index, const std::string &code, const std::string &name, TileColor color, StreetProperty *street);

    void onLanded(Player &player, Game &game) override;
};