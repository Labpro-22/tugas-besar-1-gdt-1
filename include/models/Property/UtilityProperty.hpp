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

    const std::map<int, int>& getMultiplierTable() const;
    int calculateRent(int diceValue = 0) const override;
    int getAssetValue() const override;
};
