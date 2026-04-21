#pragma once
#include "exception/PlayerTurn/PropertyManagementException.hpp"

class InsufficientMoneyException : public PropertyManagementException
{
private:
    int cost;

public:
    explicit InsufficientMoneyException(Player *player, Property *property, int cost);
};