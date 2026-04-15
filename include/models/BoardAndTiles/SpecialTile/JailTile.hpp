#pragma once
#include "models/BoardAndTiles/SpecialTile.hpp"

class JailTile : public SpecialTile
{
public:
    explicit JailTile(int index);

    void onLanded(Player &player, Game &game) override;
};