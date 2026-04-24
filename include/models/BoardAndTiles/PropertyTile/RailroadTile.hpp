#pragma once
#include "models/BoardAndTiles/PropertyTile.hpp"

class RailroadProperty;

class RailroadTile : public PropertyTile
{
public:
    RailroadTile(int index, const std::string &code, const std::string &name, RailroadProperty *railroad);

    void onLanded(Player &player, Game &game) override;
    TileKind getKind() const override { return TileKind::RAILROAD; }
};
