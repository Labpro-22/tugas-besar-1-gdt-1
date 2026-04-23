#pragma once
#include "views/viewElement/View2D.hpp"
#include "views/viewElement/Interactable.hpp"

class GameHUDView : public View2D {
private:
    Interactable switchCamBtn;
    bool isTopView = false;

public:
    GameHUDView();

    void interactionCheck() override;
    std::string catchCommand() override;
    void render() override;
};
