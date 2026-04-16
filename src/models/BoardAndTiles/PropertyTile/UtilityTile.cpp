#include "models/BoardAndTiles/PropertyTile/UtilityTile.hpp"
#include "models/Property/UtilityProperty.hpp"
// #include "core/Game.hpp"
#include "models/Player/Player.hpp"
#include "exception/PlayerTurn/PropertyManagement/InsufficientMoneyException.hpp"

UtilityTile::UtilityTile(int index, const std::string &code, const std::string &name, UtilityProperty *utility)
    : PropertyTile(index, code, name, TileColor::ABU_ABU, utility) {}

void UtilityTile::onLanded(Player &player, Game &game)
{
    // UtilityProperty *utility = static_cast<UtilityProperty *>(property);

    // // ── BANK: kepemilikan langsung tanpa bayar ───────────────
    // if (utility->getStatus() == PropertyStatus::BANK)
    // {
    //     utility->setOwner(&player);

    //     game.getUI().display("Kamu mendarat di " + name + " (" + code + ")!");
    //     game.getUI().display("Belum ada yang menginjaknya duluan, " + name + " kini menjadi milikmu!");

    //     game.getLogger().log(
    //         game.getCurrentTurn(),
    //         player.getUsername(),
    //         "UTILITY",
    //         code + " kini milik " + player.getUsername() + " (otomatis)");

    //     return;
    // }

    // // ── MORTGAGED: tidak ada sewa ────────────────────────────
    // if (utility->getStatus() == PropertyStatus::MORTGAGED)
    // {
    //     game.getUI().display("Kamu mendarat di " + name + " (" + code + "), milik " + utility->getOwner()->getUsername() + ".");
    //     game.getUI().display("Properti ini sedang digadaikan [M]. Tidak ada sewa yang dikenakan.");
    //     return;
    // }

    // // ── OWNED oleh diri sendiri: tidak ada sewa ──────────────
    // if (utility->getOwner() == &player)
    //     return;

    // // ── OWNED oleh pemain lain: sewa = dadu x faktor pengali ─
    // int diceTotal = game.getLastDiceTotal();
    // int rent = utility->calculateRent(diceTotal);

    // game.getUI().display("Kamu mendarat di " + name + " (" + code + "), milik " + utility->getOwner()->getUsername() + "!");
    // game.getUI().display("Total dadu  : " + std::to_string(diceTotal));
    // game.getUI().display("Faktor      : x" + std::to_string(utility->getMultiplier()));
    // game.getUI().display("Sewa        : M" + std::to_string(rent));

    // try
    // {
    //     player.getWallet().deduct(rent);
    //     utility->getOwner()->getWallet().receive(rent);

    //     game.getUI().display(
    //         "Uang kamu: M" + std::to_string(player.getWallet().getBalance() + rent) +
    //         " -> M" + std::to_string(player.getWallet().getBalance()));
    //     game.getUI().display(
    //         "Uang " + utility->getOwner()->getUsername() +
    //         ": M" + std::to_string(utility->getOwner()->getWallet().getBalance() - rent) +
    //         " -> M" + std::to_string(utility->getOwner()->getWallet().getBalance()));

    //     game.getLogger().log(
    //         game.getCurrentTurn(),
    //         player.getUsername(),
    //         "SEWA",
    //         "Bayar M" + std::to_string(rent) + " ke " + utility->getOwner()->getUsername() +
    //             " (" + code + ", dadu " + std::to_string(diceTotal) +
    //             " x" + std::to_string(utility->getMultiplier()) + ")");
    // }
    // catch (const InsufficientMoneyException &e)
    // {
    //     game.getUI().display("Kamu tidak mampu membayar sewa penuh! (M" + std::to_string(rent) + ")");
    //     game.getUI().display("Uang kamu saat ini: M" + std::to_string(player.getWallet().getBalance()));
    //     game.getBankruptcyHandler().handle(player, utility->getOwner(), rent);
    // }
}