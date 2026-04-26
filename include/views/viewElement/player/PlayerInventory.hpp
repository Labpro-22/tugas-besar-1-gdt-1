#pragma once

#include "views/viewElement/Popup.hpp"
#include "views/viewElement/property/PropertyPopup.hpp"
#include "models/Player/Player.hpp"
#include "models/Property/Property.hpp"

#include <memory>
#include <vector>

class PlayerInventoryPopup : public IndefinitePopup {
private:
    Player* player;
    std::unique_ptr<PropertyPopup> propertyPopup;
    std::vector<Interactable> cardUseButtons;
    std::vector<Interactable> propertyInfoButtons;
    std::string selectedPropertyCode;

public:
    PlayerInventoryPopup(Player* player);

    void enable() override;
    void disable() override;
    void interactionCheck() override;
    std::string catchCommand() override;
    void render() override;
};