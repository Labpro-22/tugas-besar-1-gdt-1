#include "models/Property/RailroadProperty.hpp"
#include "models/Player/Player.hpp"

RailroadProperty::RailroadProperty( const std::string& code,
                                    const std::string& name,
                                    int purchasePrice,
                                    int mortgageValue,
                                    const std::map<int, int>& rentTable)
    :   Property(code, name, PropertyType::RAILROAD, purchasePrice, mortgageValue), 
        rentTable(rentTable) {
}
const std::map<int, int>& RailroadProperty::getRentTable() const {
    return rentTable;
}
int RailroadProperty::calculateRent(int diceValue) const {
    if (owner == nullptr) return 0;
    int count = owner->countOwnedRailroads();
    auto it = rentTable.find(count);
    if (it != rentTable.end()) {
        return it->second * festivalMultiplier;
    }
    return 0; 
}

int RailroadProperty::getAssetValue() const {
    return purchasePrice;
}
