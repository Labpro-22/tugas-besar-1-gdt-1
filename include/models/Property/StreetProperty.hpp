#pragma once

#include "models/Property/Property.hpp"

enum class BuildingState
{
    NONE,
    HOUSE_1,
    HOUSE_2,
    HOUSE_3,
    HOUSE_4,
    HOTEL
};

class StreetProperty : public Property
{
private:
    std::string colorGroup;
    int houseBuildCost;
    int hotelBuildCost;
    std::vector<int> rentLevels;
    BuildingState buildingState;

public:
    StreetProperty(const std::string &code,
                   const std::string &name,
                   int purchasePrice,
                   int mortgageValue,
                   const std::string &colorGroup,
                   int houseBuildCost,
                   int hotelBuildCost,
                   const std::vector<int> &rentLevels);

    std::string getColorGroup() const;
    BuildingState getBuildingState() const;
    int getHouseBuildCost() const;
    int getHotelBuildCost() const;
    const std::vector<int>& getRentLevels() const;
    static int requiredStreetCount(const std::string& colorGroup);

    bool hasHotel() const;
    bool canBuildHouse() const;
    bool canBuildHotel() const;

    bool buildHouse();
    bool buildHotel();

    void clearBuildings();
    void demolishOneLevel();
    int sellBuildingValue() const;

    int calculateRent(int diceValue = 0) const override;
    int getAssetValue() const override;
};
