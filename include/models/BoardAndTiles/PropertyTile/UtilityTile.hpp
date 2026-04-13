#pragma once
#include "models/BoardAndTiles/PropertyTile.hpp"

class Utility;

class UtilityTile : public PropertyTile
{
public:
    UtilityTile(int index, const std::string &code, const std::string &name, Utility *utility);

    void onLanded(Player &player, Game &game) override;
};