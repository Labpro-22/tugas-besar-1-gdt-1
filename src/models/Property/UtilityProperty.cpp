#include "models/Property/UtilityProperty.hpp"
#include "models/Player/Player.hpp"

UtilityProperty::UtilityProperty(const std::string &code,
                                 const std::string &name,
                                 int purchasePrice,
                                 int mortgageValue,
                                 const std::map<int, int> &multiplierTable)
    : Property(code, name, PropertyType::UTILITY, purchasePrice, mortgageValue),
      multiplierTable(multiplierTable)
{
}
const std::map<int, int>& UtilityProperty::getMultiplierTable() const {
    return multiplierTable;
}
int UtilityProperty::calculateRent(int diceValue) const {
    if (owner == nullptr) return 0;
    int count = owner->countOwnedUtilities();
    auto it = multiplierTable.find(count);
    if (it != multiplierTable.end()) {
        return diceValue * it->second * festivalMultiplier;
    }
    return 0;
}

int UtilityProperty::getAssetValue() const {
    return purchasePrice;
}
