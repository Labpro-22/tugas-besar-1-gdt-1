#pragma once

#include "views/viewElement/Popup.hpp"
#include "models/Player/Player.hpp"

class PlayerInventoryPopup : public IndefinitePopup {
private:
    Player* player;

public:
    PlayerInventoryPopup(Player* player);

    void enable() override;
    void disable() override;
    void interactionCheck() override;
    std::string catchCommand() override;
    void render() override;
};