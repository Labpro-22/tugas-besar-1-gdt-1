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
    Interactable useSkillBtn;
    Interactable endTurnBtn;
    std::vector<PlayerProfileView> playerProfiles;

    bool isTopView = false;
    bool showEndTurnButton = false;
    bool showUseSkillButton = false;
    bool hideButton = false;

    const Game *gameModel = nullptr;

public:
    GameHUDView();
    PlayerProfileView* getPlayerProfile(Player* player);

    void hideButtons();
    void unhideButtons();
    void setGameModel(const Game *game);
    void updateProfileData();

    void interactionCheck() override;
    std::string catchCommand() override;
    void render() override;
};