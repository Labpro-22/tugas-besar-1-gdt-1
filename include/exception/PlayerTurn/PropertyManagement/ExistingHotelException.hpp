#pragma once
#include "exception/PlayerTurn/PropertyManagementException.hpp"

class ExistingHotelException : public PropertyManagementException
{
public:
    explicit ExistingHotelException(Player *player, Property *property);
    const char *what() const noexcept override;
};