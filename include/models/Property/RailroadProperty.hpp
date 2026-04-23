#pragma once

#include "models/Property/Property.hpp"

class RailroadProperty : public Property
{
private:
    std::map<int, int> rentTable;

public:
    RailroadProperty(const std::string &code,
                     const std::string &name,
                     int purchasePrice,
                     int mortgageValue,
                     const std::map<int, int> &rentTable);

    int calculateRent(int diceValue = 0) const override;
    int getAssetValue() const override;
};