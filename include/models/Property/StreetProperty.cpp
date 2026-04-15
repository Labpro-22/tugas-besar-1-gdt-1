#include "StreetProperty.hpp"

StreetProperty::StreetProperty( const std::string& code,
                                const std::string& name,
                                int purchasePrice,
                                int mortgageValue,
                                const std::string& colorGroup,
                                int houseBuildCost,
                                int hotelBuildCost,
                                const std::vector<int>& rentLevels)
    :   Property(code, name, PropertyType::STREET, purchasePrice, mortgageValue),
        colorGroup(colorGroup),
        houseBuildCost(houseBuildCost),
        hotelBuildCost(hotelBuildCost),
        rentLevels(rentLevels),
        buildingState(BuildingState::NONE) {
}

std::string StreetProperty::getColorGroup() const {
    return colorGroup;
}

int StreetProperty::getHouseBuildCost() const{
    return houseBuildCost;
}

int StreetProperty::getHotelBuildCost() const{
    return hotelBuildCost;
}

BuildingState StreetProperty::getBuildingState() const {
    return buildingState;
}

bool StreetProperty::hasHotel() const {
    return buildingState == BuildingState::HOTEL;
}

bool StreetProperty::canBuildHouse() const {
    return status == PropertyStatus::OWNED && buildingState != BuildingState::HOUSE_4 && buildingState != BuildingState::HOTEL;
}

bool StreetProperty::canBuildHotel() const {
    return status == PropertyStatus::OWNED && buildingState == BuildingState::HOUSE_4;
}

bool StreetProperty::buildHouse() {
    if (!canBuildHouse()) {
        return false;
    }

    switch (buildingState) {
        case BuildingState::NONE:
            buildingState = BuildingState::HOUSE_1;
            return true;
        case BuildingState::HOUSE_1:
            buildingState = BuildingState::HOUSE_2;
            return true;
        case BuildingState::HOUSE_2:
            buildingState = BuildingState::HOUSE_3;
            return true;
        case BuildingState::HOUSE_3:
            buildingState = BuildingState::HOUSE_4;
            return true;
        default:
            return false;
    }
}

bool StreetProperty::buildHotel() {
    if (!canBuildHotel()) {
        return false;
    }

    buildingState = BuildingState::HOTEL;
    return true;
}

void StreetProperty::clearBuildings() {
    buildingState = BuildingState::NONE;
}

int StreetProperty::sellBuildingValue() const {
    switch (buildingState) {
        case BuildingState::NONE:
            return 0;
        case BuildingState::HOUSE_1:
            return houseBuildCost / 2;
        case BuildingState::HOUSE_2:
            return (2 * houseBuildCost) / 2;
        case BuildingState::HOUSE_3:
            return (3 * houseBuildCost) / 2;
        case BuildingState::HOUSE_4:
            return (4 * houseBuildCost) / 2;
        case BuildingState::HOTEL:
            return ((4 * houseBuildCost) + hotelBuildCost) / 2;
        default:
            return 0;
    }
}

int StreetProperty::calculateRent(int) const {
    if (status != PropertyStatus::OWNED || owner == nullptr) {
        return 0;
    }

    int baseRent = 0;

    switch (buildingState) {
        case BuildingState::NONE:
            if (!rentLevels.empty()) {
                baseRent = rentLevels[0];
                if (owner->ownsFullColorGroup(colorGroup)) {
                    baseRent *= 2;
                }
            }
            break;
        case BuildingState::HOUSE_1:
            if (rentLevels.size() > 1) baseRent = rentLevels[1];
            break;
        case BuildingState::HOUSE_2:
            if (rentLevels.size() > 2) baseRent = rentLevels[2];
            break;
        case BuildingState::HOUSE_3:
            if (rentLevels.size() > 3) baseRent = rentLevels[3];
            break;
        case BuildingState::HOUSE_4:
            if (rentLevels.size() > 4) baseRent = rentLevels[4];
            break;
        case BuildingState::HOTEL:
            if (rentLevels.size() > 5) baseRent = rentLevels[5];
            break;
    }

    return baseRent * festivalMultiplier;
}

int StreetProperty::getAssetValue() const {
    return purchasePrice;
}

int StreetProperty::getBuildingAssetValue() const {
    return sellBuildingValue() * 2;
}