#include "exception/PlayerTurn/PropertyManagement/ExistingHotelException.hpp"

ExistingHotelException::ExistingHotelException(Player *player, Property *property)
    : PropertyManagementException(112, "Hotel Exists: ", player, property)
{
    errorMessage += "Nama Player: " + player->getUsername() +
                    ", Property: " + property->getName() +
                    " (Properti ini sudah memiliki hotel/bangunan maksimal)";
}