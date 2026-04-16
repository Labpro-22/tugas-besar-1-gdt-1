#pragma once
#include "exception/PlayerTurn/PropertyManagementException.hpp"

class PropertyNotMortgagedException : public PropertyManagementException
{
public:
    explicit PropertyNotMortgagedException();
    const char *what() const noexcept override;
};