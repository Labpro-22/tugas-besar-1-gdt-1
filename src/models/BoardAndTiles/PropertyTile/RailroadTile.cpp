#include "models/BoardAndTiles/PropertyTile/RailroadTile.hpp"
#include "models/Property/RailroadProperty.hpp"
// #include "core/Game.hpp"
#include "models/Player/Player.hpp"
#include "exception/PlayerTurn/PropertyManagement/InsufficientMoneyException.hpp"

RailroadTile::RailroadTile(int index, const std::string &code, const std::string &name, RailroadProperty *railroad)
    : PropertyTile(index, code, name, TileColor::DEFAULT, railroad) {}

void RailroadTile::onLanded(Player &player, Game &game)
{
    RailroadProperty *railroad = static_cast<RailroadProperty *>(property);

    if (railroad->getStatus() == PropertyStatus::BANK)
    {
        railroad->setOwner(&player);

        // game.getUI().display("Kamu mendarat di " + name + " (" + code + ")!");
        // game.getUI().display("Belum ada yang menginjaknya duluan, stasiun ini kini menjadi milikmu!");

        // game.getLogger().log(
        //     game.getCurrentTurn(),
        //     player.getUsername(),
        //     "RAILROAD",
        //     code + " kini milik " + player.getUsername() + " (otomatis)");

        return;
    }

    if (railroad->getStatus() == PropertyStatus::MORTGAGED ||
        railroad->getOwner() == &player)
        return;

    int rent = railroad->calculateRent();

    // game.getUI().display("Kamu mendarat di " + name + " (" + code + "), milik " + railroad->getOwner()->getUsername() + "!");
    // game.getUI().display("Sewa: M" + std::to_string(rent));

    // try
    // {
    //     player.getWallet().deduct(rent);
    //     railroad->getOwner()->getWallet().receive(rent);

    //     game.getUI().display("Uang kamu: M" + std::to_string(player.getWallet().getBalance() + rent) + " -> M" + std::to_string(player.getWallet().getBalance()));
    //     game.getUI().display("Uang " + railroad->getOwner()->getUsername() + ": M" + std::to_string(railroad->getOwner()->getWallet().getBalance() - rent) + " -> M" + std::to_string(railroad->getOwner()->getWallet().getBalance()));

    //     game.getLogger().log(
    //         game.getCurrentTurn(),
    //         player.getUsername(),
    //         "SEWA",
    //         "Bayar M" + std::to_string(rent) + " ke " + railroad->getOwner()->getUsername() + " (" + code + ")");
    // }
    // catch (const InsufficientMoneyException &e)
    // {
    //     game.getUI().display("Kamu tidak mampu membayar sewa penuh! (M" + std::to_string(rent) + ")");
    //     game.getUI().display("Uang kamu saat ini: M" + std::to_string(player.getWallet().getBalance()));
    //     game.getBankruptcyHandler().handle(player, railroad->getOwner(), rent);
    // }
}