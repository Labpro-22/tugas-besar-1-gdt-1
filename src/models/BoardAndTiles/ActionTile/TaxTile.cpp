#include "models/BoardAndTiles/ActionTile/TaxTile.hpp"
// #include "core/Game.hpp"
// #include "models/Player/Player.hpp"
// #include "exception/InsufficientFundsException.hpp"

TaxTile::TaxTile(int index, const std::string &code, const std::string &name)
    : ActionTile(index, code, name) {}

void TaxTile::onLanded(Player &player, Game &game)
{
    // int amount = calculateTax(player);

    // game.getUI().display(
    //     "Pajak yang harus dibayar: M" + std::to_string(amount));

    // try
    // {
    //     player.getWallet().deduct(amount);

    //     game.getLogger().log(
    //         game.getCurrentTurn(),
    //         player.getUsername(),
    //         "PAJAK",
    //         "Bayar M" + std::to_string(amount) + " ke Bank (" + name + ")");
    // }
    // catch (const InsufficientFundsException &e)
    // {
    //     game.getUI().display(e.what());
    //     game.getBankruptcyHandler().handle(player, nullptr, amount);
    // }
}