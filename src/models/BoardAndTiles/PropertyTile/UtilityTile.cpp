#include "models/BoardAndTiles/PropertyTile/UtilityTile.hpp"
#include "models/Property/UtilityProperty.hpp"
#include "core/Game.hpp"
#include "models/Player/Player.hpp"
#include "exception/PlayerTurn/PropertyManagement/InsufficientMoneyException.hpp"
#include <iostream>

UtilityTile::UtilityTile(int index, const std::string &code, const std::string &name, UtilityProperty *utility)
    : PropertyTile(index, code, name, TileColor::ABU_ABU, utility) {}

void UtilityTile::onLanded(Player &player, Game &game)
{
    UtilityProperty *utility = static_cast<UtilityProperty *>(property);

    if (utility->getStatus() == PropertyStatus::BANK) {
        std::cout << player.getUsername() << " mendarat di utilitas " << utility->getName()
                  << " yang belum dimiliki siapa pun.\n";
        return;
    }

    if (utility->getStatus() == PropertyStatus::MORTGAGED || utility->getOwner() == &player)
        return;

    int diceTotal = game.getLastDiceTotal();
    int rent = utility->calculateRent(diceTotal);

    std::cout << player.getUsername() << " mendarat di " << utility->getName()
              << " milik " << utility->getOwner()->getUsername()
              << ". Sewa: " << rent << " (dadu " << diceTotal << ").\n";

    if (!player.canAfford(rent)) {
        throw InsufficientMoneyException(&player, utility, rent);
    }

    player.deductMoney(rent);
    utility->getOwner()->addMoney(rent);
}