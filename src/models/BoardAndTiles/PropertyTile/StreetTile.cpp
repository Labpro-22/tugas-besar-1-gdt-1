#include "models/BoardAndTiles/PropertyTile/StreetTile.hpp"
#include "models/Property/StreetProperty.hpp"
// #include "core/Game.hpp"
#include "models/Player/Player.hpp"
#include "exception/PlayerTurn/PropertyManagement/InsufficientMoneyException.hpp"

StreetTile::StreetTile(int index, const std::string &code, const std::string &name, TileColor color, StreetProperty *street)
    : PropertyTile(index, code, name, color, street) {}

void StreetTile::onLanded(Player &player, Game &game)
{
    // StreetProperty *street = static_cast<StreetProperty *>(property);

    // // ── BANK: tawarkan pembelian ─────────────────────────────
    // if (street->getStatus() == PropertyStatus::BANK)
    // {
    //     game.getUI().display("Kamu mendarat di " + name + " (" + code + ")!");
    //     game.getUI().displayDeed(street);
    //     game.getUI().display("Uang kamu saat ini: M" + std::to_string(player.getWallet().getBalance()));
    //     game.getUI().display("Apakah kamu ingin membeli properti ini seharga M" + std::to_string(street->getPrice()) + "? (y/n): ");

    //     std::string choice = game.getUI().getInput();

    //     if (choice == "y" || choice == "Y")
    //     {
    //         try
    //         {
    //             player.getWaller().deduct(street->getPrice());
    //             street->setOwner(&player);

    //             game.getUI().display(name + " kini menjadi milikmu!");
    //             game.getUI().display("Uang tersisa: M" + std::to_string(player.getWallet().getBalance()));

    //             game.getLogger().log(
    //                 game.getCurrentTurn(),
    //                 player.getUsername(),
    //                 "BELI",
    //                 "Beli " + name + " (" + code + ") seharga M" + std::to_string(street->getPrice()));
    //         }
    //         catch (const InsufficientMoneyException &e)
    //         {
    //             game.getUI().display("Uang tidak cukup untuk membeli properti ini!");
    //             game.getUI().display("Properti ini akan masuk ke sistem lelang...");
    //             game.getAuctionSystem().auction(street, player, game);
    //         }
    //     }
    //     else
    //     {
    //         game.getUI().display("Properti ini akan masuk ke sistem lelang...");
    //         game.getAuctionSystem().auction(street, player, game);
    //     }

    //     return;
    // }

    // // ── MORTGAGED: tidak ada sewa ────────────────────────────
    // if (street->getStatus() == PropertyStatus::MORTGAGED)
    // {
    //     game.getUI().display("Kamu mendarat di " + name + " (" + code + "), milik " + street->getOwner()->getUsername() + ".");
    //     game.getUI().display("Properti ini sedang digadaikan [M]. Tidak ada sewa yang dikenakan.");
    //     return;
    // }

    // // ── OWNED oleh diri sendiri: tidak ada sewa ──────────────
    // if (street->getOwner() == &player)
    //     return;

    // // ── OWNED oleh pemain lain: bayar sewa ───────────────────
    // int rent = street->calculateRent();

    // game.getUI().display("Kamu mendarat di " + name + " (" + code + "), milik " + street->getOwner()->getUsername() + "!");
    // game.getUI().display("Kondisi  : " + street->getBuildingLabel());
    // game.getUI().display("Sewa     : M" + std::to_string(rent));

    // try
    // {
    //     player.getWallet().deduct(rent);
    //     street->getOwner()->getWallet().receive(rent);

    //     game.getUI().display(
    //         "Uang kamu: M" + std::to_string(player.getWallet().getBalance() + rent) +
    //         " -> M" + std::to_string(player.getWallet().getBalance()));
    //     game.getUI().display(
    //         "Uang " + street->getOwner()->getUsername() +
    //         ": M" + std::to_string(street->getOwner()->getWallet().getBalance() - rent) +
    //         " -> M" + std::to_string(street->getOwner()->getWallet().getBalance()));

    //     game.getLogger().log(
    //         game.getCurrentTurn(),
    //         player.getUsername(),
    //         "SEWA",
    //         "Bayar M" + std::to_string(rent) + " ke " + street->getOwner()->getUsername() +
    //             " (" + code + ", " + street->getBuildingLabel() + ")");
    // }
    // catch (const InsufficientMoneyException &e)
    // {
    //     game.getUI().display("Kamu tidak mampu membayar sewa penuh! (M" + std::to_string(rent) + ")");
    //     game.getUI().display("Uang kamu saat ini: M" + std::to_string(player.getWallet().getBalance()));
    //     game.getBankruptcyHandler().handle(player, street->getOwner(), rent);
    // }
}