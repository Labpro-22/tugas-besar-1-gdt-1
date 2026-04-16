#pragma once
#include "models/BoardAndTiles/PropertyTile.hpp"

class UtilityProperty;

class UtilityTile : public PropertyTile
{
public:
    UtilityTile(int index, const std::string &code, const std::string &name, UtilityProperty *utility);

    void onLanded(Player &player, Game &game) override;
};