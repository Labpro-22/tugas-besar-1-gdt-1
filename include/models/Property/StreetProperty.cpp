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