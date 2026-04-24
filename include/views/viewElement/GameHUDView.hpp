#pragma once
#include "views/viewElement/View2D.hpp"
#include "views/viewElement/Interactable.hpp"

class GameHUDView : public View2D {
private:
    Interactable switchCamBtn;
    Interactable rollDiceBtn;

    bool isTopView = false;
    int currentPlayerIdx = 1;

public:
    GameHUDView();

    void setCurrentPlayerIdx(int idx);

    void interactionCheck() override;
    std::string catchCommand() override;
    void render() override;
};