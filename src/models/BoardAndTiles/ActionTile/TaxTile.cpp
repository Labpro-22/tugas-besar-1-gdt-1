#include "models/BoardAndTiles/ActionTile/TaxTile.hpp"
#include "models/Player/Player.hpp"

TaxTile::TaxTile(int index, const std::string &code, const std::string &name)
    : ActionTile(index, code, name) {}

void TaxTile::onLanded(Player &player, Game &game)
{
    int amount = calculateTax(player);
    if (!player.deductMoney(amount)) {
        player.setStatus(PlayerStatus::BANKRUPT);
    }
}