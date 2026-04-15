#pragma once
#include "models/BoardAndTiles/SpecialTile.hpp"

class GoToJailTile : public SpecialTile
{
public:
    explicit GoToJailTile(int index);

    void onLanded(Player &player, Game &game) override;
};