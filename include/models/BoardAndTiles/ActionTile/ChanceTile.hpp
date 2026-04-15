#pragma once
#include "models/BoardAndTiles/ActionTile.hpp"

class ChanceTile : public ActionTile
{
public:
    explicit ChanceTile(int index);

    void onLanded(Player &player, Game &game) override;
};