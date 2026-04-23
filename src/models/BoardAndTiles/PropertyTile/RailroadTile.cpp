#include "models/BoardAndTiles/PropertyTile/RailroadTile.hpp"
#include "models/Property/RailroadProperty.hpp"
#include "models/Player/Player.hpp"
#include "core/Game.hpp"
#include "utils/data/TransactionLogger.hpp"
#include "exception/PlayerTurn/PropertyManagement/InsufficientMoneyException.hpp"

RailroadTile::RailroadTile(int index, const std::string &code, const std::string &name, RailroadProperty *railroad)
    : PropertyTile(index, code, name, TileColor::DEFAULT, railroad) {}

void RailroadTile::onLanded(Player &player, Game &game)
{
    RailroadProperty *railroad = static_cast<RailroadProperty *>(property);

    if (railroad->getStatus() == PropertyStatus::BANK)
    {
        railroad->setOwner(&player);

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

    // try
    // {
    //     player.getWallet().deduct(rent);
    //     railroad->getOwner()->getWallet().receive(rent);

    //     game.getLogger().log(
    //         game.getCurrentTurn(),
    //         player.getUsername(),
    //         "SEWA",
    //         "Bayar M" + std::to_string(rent) + " ke " + railroad->getOwner()->getUsername() + " (" + code + ")");
    // }
    // catch (const InsufficientMoneyException &e)
    // {
    //     game.getBankruptcyHandler().handle(player, railroad->getOwner(), rent);
    // }
}