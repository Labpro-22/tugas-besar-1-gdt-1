#include "exception/PlayerTurn/PropertyManagement/ExistingHotelException.hpp"

ExistingHotelException::ExistingHotelException(Player *player, Property *property)
    : PropertyManagementException(111, "Hotel Exists: ", player, property) {};

const char *ExistingHotelException::what() const noexcept
{
    return errorMessage.c_str();
};