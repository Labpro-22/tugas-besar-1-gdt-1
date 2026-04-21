#pragma once
#include "exception/PlayerTurn/PropertyManagementException.hpp"

class InsufficientOwnedColorException : public PropertyManagementException
{
public:
    explicit InsufficientOwnedColorException(Player *player, Property *property);
};