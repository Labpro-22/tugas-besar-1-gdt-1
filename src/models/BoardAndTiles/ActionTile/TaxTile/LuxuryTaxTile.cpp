#include "models/BoardAndTiles/ActionTile/TaxTile/LuxuryTaxTile.hpp"
// #include "models/Player/Player.hpp"

LuxuryTaxTile::LuxuryTaxTile(int index, int flatAmount)
    : TaxTile(index, "PBM", "Pajak Barang Mewah"),
      flatAmount(flatAmount) {}

int LuxuryTaxTile::calculateTax(const Player &player) const
{
    return flatAmount;
}