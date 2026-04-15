#include "UtilityProperty.hpp"

UtilityProperty::UtilityProperty(   const std::string& code,
                                    const std::string& name,
                                    int purchasePrice,
                                    int mortgageValue,
                                    const std::map<int, int>& multiplierTable)
    :   Property(code, name, PropertyType::UTILITY, purchasePrice, mortgageValue),
        multiplierTable(multiplierTable) {
}

int UtilityProperty::calculateRent(int diceValue) const {
    if (status != PropertyStatus::OWNED || owner == nullptr) {
        return 0;
    }

    int utilityCount = owner->countOwnedUtilities();
    auto it = multiplierTable.find(utilityCount);

    if (it != multiplierTable.end()) {
        return diceValue * it->second * festivalMultiplier;
    }

    return 0;
}

int UtilityProperty::getAssetValue() const {
    return purchasePrice;
}