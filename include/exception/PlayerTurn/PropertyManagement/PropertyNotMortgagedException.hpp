#pragma once
#include "exception/PlayerTurn/PropertyManagementException.hpp"

class PropertyNotMortgagedException : public PropertyManagementException
{
public:
    explicit PropertyNotMortgagedException(Player *player, Property *property);
    const char *what() const noexcept override;
};