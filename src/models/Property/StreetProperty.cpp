#include "models/Property/StreetProperty.hpp"

StreetProperty::StreetProperty(const std::string &code,
                               const std::string &name,
                               int purchasePrice,
                               int mortgageValue,
                               const std::string &colorGroup,
                               int houseBuildCost,
                               int hotelBuildCost,
                               const std::vector<int> &rentLevels)
    : Property(code, name, PropertyType::STREET, purchasePrice, mortgageValue),
      colorGroup(colorGroup),
      houseBuildCost(houseBuildCost),
      hotelBuildCost(hotelBuildCost),
      rentLevels(rentLevels),
      buildingState(BuildingState::NONE)
{
}
std::string StreetProperty::getColorGroup() const { return colorGroup; }

BuildingState StreetProperty::getBuildingState() const { return buildingState; }
int StreetProperty::getHouseBuildCost() const { return houseBuildCost; }
int StreetProperty::getHotelBuildCost() const { return hotelBuildCost; }
const std::vector<int>& StreetProperty::getRentLevels() const { return rentLevels; }

bool StreetProperty::hasHotel() const {
    return buildingState == BuildingState::HOTEL;
}

bool StreetProperty::canBuildHouse() const {
    return status == PropertyStatus::OWNED && buildingState < BuildingState::HOUSE_4;
}

bool StreetProperty::canBuildHotel() const {
    return status == PropertyStatus::OWNED && buildingState == BuildingState::HOUSE_4;
}

bool StreetProperty::buildHouse() {
    if (canBuildHouse()) {
        buildingState = static_cast<BuildingState>(static_cast<int>(buildingState) + 1);
        return true;
    }
    return false;
}

bool StreetProperty::buildHotel() {
    if (canBuildHotel()) {
        buildingState = BuildingState::HOTEL;
        return true;
    }
    return false;
}

void StreetProperty::clearBuildings() {
    buildingState = BuildingState::NONE;
}

int StreetProperty::sellBuildingValue() const {
    int buildingValue = 0;
    if (buildingState == BuildingState::HOTEL) {
        buildingValue = (houseBuildCost * 4 + hotelBuildCost) / 2;
    } else {
        buildingValue = (houseBuildCost * static_cast<int>(buildingState)) / 2;
    }
    return buildingValue;
}

int StreetProperty::calculateRent(int diceValue) const {
    if (rentLevels.empty()) return 0;
    size_t stateIdx = static_cast<size_t>(buildingState); 
    if (stateIdx < rentLevels.size()) {
        return rentLevels[stateIdx] * festivalMultiplier;
    }
    return rentLevels.back() * festivalMultiplier;
}

int StreetProperty::getAssetValue() const {
    int buildingValue = 0;
    if (buildingState == BuildingState::HOTEL) {
        buildingValue = houseBuildCost * 4 + hotelBuildCost;
    } else {
        buildingValue = houseBuildCost * static_cast<int>(buildingState);
    }
    return purchasePrice + buildingValue;
}
