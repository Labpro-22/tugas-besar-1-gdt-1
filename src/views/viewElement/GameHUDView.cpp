#include "views/viewElement/GameHUDView.hpp"

GameHUDView::GameHUDView()
    : View2D({0, 0}, {0, 0}, []() {}),
      switchCamBtn(
          {180, 50},
          true,
          false,
          "DISPLAY TOP_VIEW 1",
          []() {},
          [this]()
          {
              if (!isTopView)
                  switchCamBtn.setGameCommand("DISPLAY TOP_VIEW 1");
              else
                  switchCamBtn.setGameCommand("DISPLAY BOARD_CAM 1");

              isTopView = !isTopView;
          })
{
}

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
    float margin = 20.0f;
    float w = 180.0f;
    float h = 50.0f;

    float x = GetScreenWidth() - w - margin;
    float y = GetScreenHeight() - h - margin;

    switchCamBtn.movePosition({x + w / 2, y + h / 2});

    DrawRectangleRounded({x + 3, y + 3, w, h}, 0.3f, 8, Fade(BLACK, 0.4f));

    Color base = isTopView ? Color{40, 120, 220, 255} : Color{40, 180, 120, 255};

    DrawRectangleRounded({x, y, w, h}, 0.3f, 8, base);

    DrawRectangleRoundedLines({x, y, w, h}, 0.3f, 8, WHITE);

    const char *text = isTopView ? "BOARD VIEW" : "TOP VIEW";

    int fontSize = 18;
    int textWidth = MeasureText(text, fontSize);

    DrawText(
        text,
        x + (w - textWidth) / 2,
        y + (h - fontSize) / 2,
        fontSize,
        WHITE);

    switchCamBtn.render();
}