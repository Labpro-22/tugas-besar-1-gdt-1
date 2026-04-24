#pragma once
#include "models/BoardAndTiles/ActionTile/TaxTile.hpp"

class IncomeTaxTile : public TaxTile
{
private:
    int flatAmount;
    int percentage;

public:
    IncomeTaxTile(int index, int flatAmount, int percentage);
    int getFlatAmount() const;
    int getTaxPercentage() const;
    int calculateTax(const Player &player) const override;
    void onLanded(Player &player, Game &game) override;
    TileKind getKind() const override { return TileKind::INCOME_TAX; }
};
