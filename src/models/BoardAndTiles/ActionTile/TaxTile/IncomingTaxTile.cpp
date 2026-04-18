#include "models/BoardAndTiles/ActionTile/TaxTile/IncomingTaxTile.hpp"
// #include "core/Game.hpp"
#include "models/Player/Player.hpp"
#include "exception/PlayerTurn/PropertyManagement/InsufficientMoneyException.hpp"

IncomeTaxTile::IncomeTaxTile(int index, int flatAmount, int percentage)
    : TaxTile(index, "PPH", "Pajak Penghasilan"),
      flatAmount(flatAmount),
      percentage(percentage) {}

int IncomeTaxTile::calculateTax(const Player &player) const
{
    return player.calculateTotalWealth() * percentage / 100;
}

void IncomeTaxTile::onLanded(Player &player, Game &game)
{
    int amountPercent = calculateTax(player);
    // Standard rule: Choose whichever is lower or prompt (simplifying to prompt-like logic if possible, 
    // but here we'll just deduct the flat if it's smaller, or vice versa)
    int amountToPay = (flatAmount < amountPercent) ? flatAmount : amountPercent;
    
    if (!player.deductMoney(amountToPay)) {
        player.setStatus(PlayerStatus::BANKRUPT);
    }
}