#pragma once

#include "models/Property/Property.hpp"

class UtilityProperty : public Property
{
private:
    std::map<int, int> multiplierTable;

public:
    UtilityProperty(const std::string &code,
                    const std::string &name,
                    int purchasePrice,
                    int mortgageValue,
                    const std::map<int, int> &multiplierTable);

    int calculateRent(int diceValue = 0) const override;
    int getAssetValue() const override;
};