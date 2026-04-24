#pragma once
#include "models/BoardAndTiles/ActionTile/TaxTile.hpp"

class LuxuryTaxTile : public TaxTile
{
private:
    int flatAmount;

public:
    LuxuryTaxTile(int index, int flatAmount);
    int getFlatAmount() const;
    int calculateTax(const Player &player) const override;
    TileKind getKind() const override { return TileKind::LUXURY_TAX; }
};
