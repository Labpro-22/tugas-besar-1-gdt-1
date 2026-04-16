#pragma once
#include "exception/PlayerTurn/PropertyManagementException.hpp"

class ExistingHotelException : public PropertyManagementException
{
public:
    explicit ExistingHotelException();
    const char *what() const noexcept override;
};