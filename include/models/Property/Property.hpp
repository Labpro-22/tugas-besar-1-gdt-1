#pragma once

#include "models/Player/Player.hpp"
#include <string>
#include <vector>
#include <map>

enum class PropertyStatus
{
    BANK,
    OWNED,
    MORTGAGED
};

enum class PropertyType
{
    STREET,
    RAILROAD,
    UTILITY
};

class Property
{
protected:
    std::string code;
    std::string name;
    PropertyType type;
    int purchasePrice;
    int mortgageValue;
    Player *owner;
    PropertyStatus status;
    int festivalMultiplier;
    int festivalDuration;

public:
    Property(const std::string &code,
             const std::string &name,
             PropertyType type,
             int purchasePrice,
             int mortgageValue);

    virtual ~Property() = default;

    std::string getCode() const;
    std::string getName() const;
    PropertyType getType() const;
    Player *getOwner() const;
    PropertyStatus getStatus() const;
    int getPurchasePrice() const;
    int getMortgageValue() const;
    int getFestivalMultiplier() const;
    int getFestivalDuration() const;

    void setOwner(Player *owner);
    void clearOwner();
    void setStatus(PropertyStatus status);

    bool isOwned() const;
    bool isMortgaged() const;

    void activateFestival();
    void decrementFestivalDuration();

    virtual int calculateRent(int diceValue = 0) const = 0;
    virtual int getAssetValue() const = 0;
    virtual int getBuildingAssetValue() const; //tambahan

    std::string to_string(PropertyStatus status)
    {
        switch (status)
        {
        case PropertyStatus::BANK:
            return "BANK";
        case PropertyStatus::OWNED:
            return "OWNED";
        case PropertyStatus::MORTGAGED:
            return "MORTGAGED";
        default:
            return "UNKNOWN";
        }
    }
};
