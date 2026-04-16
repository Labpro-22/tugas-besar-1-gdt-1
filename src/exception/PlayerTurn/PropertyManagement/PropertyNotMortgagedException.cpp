#include "exception/PlayerTurn/PropertyManagement/PropertyNotMortgagedException.hpp"

PropertyNotMortgagedException::PropertyNotMortgagedException(Player *player, Property *property)
    : PropertyManagementException(114, "Property Not Mortgaged: ", player, property) {};

const char *PropertyNotMortgagedException::what() const noexcept
{
    return errorMessage.c_str();
};