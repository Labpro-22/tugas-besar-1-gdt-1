#include "exception/PlayerTurn/PropertyManagement/InsufficientMoneyException.hpp"

InsufficientMoneyException::InsufficientMoneyException(Player *player, Property *property, int cost)
    : PropertyManagementException(111, "Error Property: ", player, property), cost(cost) {};

const char *InsufficientMoneyException::what() const noexcept
{
    return errorMessage.c_str();
};