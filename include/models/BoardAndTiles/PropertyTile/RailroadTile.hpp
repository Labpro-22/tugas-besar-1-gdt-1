#pragma once
#include "models/BoardAndTiles/PropertyTile.hpp"

class Railroad;

class RailroadTile : public PropertyTile
{
public:
    RailroadTile(int index, const std::string &code, const std::string &name, Railroad *railroad);

    void onLanded(Player &player, Game &game) override;
};