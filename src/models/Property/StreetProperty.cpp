#include "models/Property/StreetProperty.hpp"
#include "models/Player/Player.hpp"

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

int StreetProperty::requiredStreetCount(const std::string& colorGroup) {
    if (colorGroup == "COKLAT" || colorGroup == "BIRU_TUA") {
        return 2;
    }

    if (colorGroup == "BIRU_MUDA" || colorGroup == "MERAH_MUDA" ||
        colorGroup == "ORANGE" || colorGroup == "MERAH" ||
        colorGroup == "KUNING" || colorGroup == "HIJAU") {
        return 3;
    }

    return 0;
}

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

void StreetProperty::demolishOneLevel() {
    if (buildingState == BuildingState::NONE) return;
    buildingState = static_cast<BuildingState>(static_cast<int>(buildingState) - 1);
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

int StreetProperty::calculateRent(int /*diceValue*/) const {
    if (rentLevels.empty()) return 0;

    size_t stateIdx = static_cast<size_t>(buildingState);
    int rent = 0;
    if (stateIdx < rentLevels.size()) {
        rent = rentLevels[stateIdx];
    } else {
        rent = rentLevels.back();
    }

    if (buildingState == BuildingState::NONE && owner != nullptr) {
        const int required = StreetProperty::requiredStreetCount(colorGroup);
        int ownedCount = 0;
        for (Property *property : owner->getOwnedProperties()) {
            if (!property->isStreet()) continue;
            auto *street = static_cast<StreetProperty *>(property);
            if (street->getColorGroup() == colorGroup) {
                ++ownedCount;
            }
        }

        if (required > 0 && ownedCount == required) {
            rent *= 2;
        }
    }

    return rent * festivalMultiplier;
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
