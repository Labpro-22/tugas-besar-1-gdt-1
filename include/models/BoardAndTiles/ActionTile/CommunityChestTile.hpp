#pragma once
#include "models/BoardAndTiles/ActionTile.hpp"

class CommunityChestTile : public ActionTile
{
public:
    explicit CommunityChestTile(int index);

    void onLanded(Player &player, Game &game) override;
};