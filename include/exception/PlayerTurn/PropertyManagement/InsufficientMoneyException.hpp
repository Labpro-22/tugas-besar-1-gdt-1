#pragma once
#include "exception/PlayerTurn/PropertyManagementException.hpp"

class InsufficientMoneyException : public PropertyManagementException
{
private:
    int cost;

public:
    explicit InsufficientMoneyException(const int &cost);
    const char *what() const noexcept override;
};