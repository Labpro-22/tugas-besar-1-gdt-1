#include "models/BoardAndTiles/PropertyTile/RailroadTile.hpp"
#include "models/Property/RailroadProperty.hpp"
#include "models/Player/Player.hpp"
#include "core/Game.hpp"
#include "exception/PlayerTurn/PropertyManagement/InsufficientMoneyException.hpp"
#include <iostream>

RailroadTile::RailroadTile(int index, const std::string &code, const std::string &name, RailroadProperty *railroad)
    : PropertyTile(index, code, name, TileColor::DEFAULT, railroad) {}

void RailroadTile::onLanded(Player &player, Game & /*game*/)
{
    RailroadProperty *railroad = static_cast<RailroadProperty *>(property);

    if (railroad->getStatus() == PropertyStatus::BANK) {
        std::cout << player.getUsername() << " mendarat di stasiun " << railroad->getName()
                  << " yang belum dimiliki siapa pun.\n";
        return;
    }

    if (railroad->getStatus() == PropertyStatus::MORTGAGED || railroad->getOwner() == &player)
        return;

    int rent = railroad->calculateRent(player.countOwnedRailroads());

    if (!player.canAfford(rent)) {
        throw InsufficientMoneyException(&player, railroad, rent);
    }

    player.deductMoney(rent);
    railroad->getOwner()->addMoney(rent);
    std::cout << player.getUsername() << " membayar sewa stasiun " << railroad->getName()
              << " sebesar " << rent << " ke " << railroad->getOwner()->getUsername() << ".\n";
}