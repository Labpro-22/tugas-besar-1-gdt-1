#include "exception/PlayerTurn/PropertyManagement/PropertyNotMortgagedException.hpp"

PropertyNotMortgagedException::PropertyNotMortgagedException(Player *player, Property *property)
    : PropertyManagementException(114, "Property Not Mortgaged: ", player, property)
{
    errorMessage += "Nama Player: " + player->getUsername() +
                    ", Property: " + property->getName() +
                    " (Status properti ini tidak sedang digadaikan, sehingga tidak bisa ditebus)";
}