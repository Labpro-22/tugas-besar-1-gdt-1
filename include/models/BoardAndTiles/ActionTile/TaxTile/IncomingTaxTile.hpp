#pragma once
#include "models/BoardAndTiles/ActionTile/TaxTile.hpp"

class IncomeTaxTile : public TaxTile
{
private:
    int flatAmount;
    int percentage;

public:
    IncomeTaxTile(int index, int flatAmount, int percentage);

    int calculateTax(const Player &player) const override;
    void onLanded(Player &player, Game &game) override;
};