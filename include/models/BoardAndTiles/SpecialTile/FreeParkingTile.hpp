#pragma once
#include "models/BoardAndTiles/SpecialTile.hpp"

class FreeParkingTile : public SpecialTile
{
public:
    explicit FreeParkingTile(int index);

    void onLanded(Player &player, Game &game) override;
    TileKind getKind() const override { return TileKind::FREE_PARKING; }
};
