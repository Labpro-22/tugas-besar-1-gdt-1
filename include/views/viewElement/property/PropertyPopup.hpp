#pragma once

#include "views/viewElement/Popup.hpp"

#include <string>
#include <vector>

class PropertyPopup : public IndefinitePopup
{
private:
    std::string name;
    std::string type;
    std::string status;

    int buyPrice;
    int mortgageValue;

    int levelOrCount = 0;
    bool isOtherPlayer = false;

    std::string ownerName;

    std::string colorGroup;
    std::vector<int> rentTable;
    int buildCost;

    std::vector<int> railroadRent;
    std::vector<int> utilityMultiplier;

    std::string actionCommand;
    std::vector<Interactable> actionButtons;

    std::string buildDetails() const;

public:
    PropertyPopup(
        const std::string &name,
        const std::string &type,
        const std::string &status,
        int buyPrice,
        int mortgageValue,
        int levelOrCount = 0,
        bool isOtherPlayer = false,
        const std::string &ownerName = "",
        const std::string &colorGroup = "",
        const std::vector<int> &rentTable = {},
        int buildCost = 0,
        const std::vector<int> &railroadRent = {},
        const std::vector<int> &utilityMultiplier = {});

    void enable() override;
    void disable() override;
    void interactionCheck() override;
    void addButton(const std::string &label, const std::string &command);
    std::string catchCommand() override;
    void render() override;
};
