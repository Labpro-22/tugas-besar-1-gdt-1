#pragma once
#include "exception/PlayerTurnException.hpp"
#include "models/Property/Property.hpp"

class PropertyManagementException : public PlayerTurnException
{
protected:
    Property *property;

public:
    explicit PropertyManagementException(const Property &property);
};