#include "exception/PlayerTurn/PropertyManagement/InsufficientOwnedColorException.hpp"

InsufficientOwnedColorException::InsufficientOwnedColorException(Player *player, Property *property)
    : PropertyManagementException(113, "Insufficient Color: ", player, property)
{
    errorMessage += "Nama Player: " + player->getUsername() +
                    ", Property: " + property->getName() +
                    " (Player belum memiliki semua properti dengan warna/komplek ini)";
}