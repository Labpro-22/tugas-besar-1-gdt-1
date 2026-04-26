#pragma once
#include "views/viewElement/View2D.hpp"
#include "views/viewElement/Interactable.hpp"
#include "views/viewElement/player/PlayerProfileView.hpp"

class Game;

class GameHUDView : public View2D
{
private:
    Interactable switchCamBtn;
    Interactable rollDiceBtn;
    Interactable endTurnBtn;
    std::vector<PlayerProfileView> playerProfiles;

    bool isTopView = false;
    bool showEndTurnButton = false;
    bool diceAnimationFinished = true;

    const Game *gameModel = nullptr;

public:
    GameHUDView();
    
    void setGameModel(const Game *game);
    void setDiceAnimationFinished(bool finished);
    void updateProfileData();

    void interactionCheck() override;
    std::string catchCommand() override;
    void render() override;
};