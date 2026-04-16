#pragma once
#include "exception/PlayerTurn/PropertyManagementException.hpp"

class InsufficientOwnedColorException : public PropertyManagementException
{
public:
    explicit InsufficientOwnedColorException();
    const char *what() const noexcept override;
};