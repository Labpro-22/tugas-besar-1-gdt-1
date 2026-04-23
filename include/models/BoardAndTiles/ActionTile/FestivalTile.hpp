#pragma once
#include "models/BoardAndTiles/ActionTile.hpp"

class FestivalTile : public ActionTile
{
public:
    explicit FestivalTile(int index);

    void onLanded(Player &player, Game &game) override;
};