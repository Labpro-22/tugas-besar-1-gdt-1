#include "exception/PlayerTurn/PropertyManagement/InsufficientMoneyException.hpp"

InsufficientMoneyException::InsufficientMoneyException(Player *player, Property *property, int cost)
    : PropertyManagementException(111, "Uang tidak cukup: ", player, property), cost(cost)
{
    errorMessage += "Nama Player: " + player->getUsername() +
                    ", Property: " + property->getName() +
                    ", Harga: " + std::to_string(cost) +
                    " (Uang saat ini: " + std::to_string(player->getBalance()) + ")";
}