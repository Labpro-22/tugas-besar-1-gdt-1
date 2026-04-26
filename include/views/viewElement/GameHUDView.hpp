#pragma once
#include "views/viewElement/View2D.hpp"
#include "views/viewElement/Interactable.hpp"
#include "views/viewElement/player/PlayerProfileView.hpp"
#include "views/viewElement/player/PlayerInventory.hpp"

#include <memory>

class Game;

class GameHUDView : public View2D
{
private:
    Interactable switchCamBtn;
    Interactable rollDiceBtn;
    Interactable endTurnBtn;
    std::vector<PlayerProfileView> playerProfiles;
    std::unique_ptr<PlayerInventoryPopup> inventoryPopup;

    bool isTopView = false;
    bool showEndTurnButton = false;

    const Game *gameModel = nullptr;

public:
    GameHUDView();
    
    void setGameModel(const Game *game);
    void updateProfileData();

    void interactionCheck() override;
    std::string catchCommand() override;
    void render() override;
};