#pragma once
#include "models/BoardAndTiles/SpecialTile.hpp"

class GoTile : public SpecialTile
{
private:
    int salary;

public:
    GoTile(int index, int salary);
    int getSalary() const;

    void onLanded(Player &player, Game &game) override;
    void onPassed(Player &player, Game &game) override;
    TileKind getKind() const override { return TileKind::GO; }
};
