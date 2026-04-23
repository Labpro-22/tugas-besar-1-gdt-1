#include "views/viewElement/GameHUDView.hpp"

GameHUDView::GameHUDView()
: View2D({100,100}, {200,50}, [](){}),
  switchCamBtn(
    {200,50},
    true,
    false,
    "DISPLAY TOP_VIEW 1",
    [](){},
    [this]()
    {
        if (!isTopView)
            switchCamBtn.setGameCommand("DISPLAY TOP_VIEW 1");
        else
            switchCamBtn.setGameCommand("DISPLAY BOARD_CAM 1");

        isTopView = !isTopView;
    }
)
{}

void GameHUDView::interactionCheck()
{
    switchCamBtn.interactionCheck();
}

std::string GameHUDView::catchCommand()
{
    return switchCamBtn.catchCommand();
}

void GameHUDView::render()
{
    switchCamBtn.render();
}