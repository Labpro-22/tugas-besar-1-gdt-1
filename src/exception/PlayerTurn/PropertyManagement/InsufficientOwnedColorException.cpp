#include "exception/PlayerTurn/PropertyManagement/InsufficientOwnedColorException.hpp"

InsufficientOwnedColorException::InsufficientOwnedColorException(Player *player, Property *property)
    : PropertyManagementException(113, "Insufficient Color: ", player, property) {};

const char *InsufficientOwnedColorException::what() const noexcept
{
    return errorMessage.c_str();
};