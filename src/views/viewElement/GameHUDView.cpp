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
    float w = 200.0f;
    float h = 56.0f;

    float x = GetScreenWidth() - w - margin;
    float y = (GetScreenHeight() - h) / 2.0f;

    switchCamBtn.movePosition({x + w / 2, y + h / 2});

    DrawRectangleRounded({x + 4, y + 6, w, h}, 0.5f, 8, Fade(BLACK, 0.25f));
    DrawRectangleRounded({x + 2, y + 3, w, h}, 0.5f, 8, Fade(BLACK, 0.15f));

    Color base = isTopView
        ? Color{30, 110, 210, 255}
        : Color{30, 170, 110, 255};

    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, {x, y, w, h});

    if (hover)
    {
        base = Color{
            (unsigned char)std::min(base.r + 20, 255),
            (unsigned char)std::min(base.g + 20, 255),
            (unsigned char)std::min(base.b + 20, 255),
            255};
    }

    DrawRectangleRounded({x, y, w, h}, 0.5f, 8, base);

    DrawRectangleRoundedLines({x, y, w, h}, 0.5f, 8, Fade(WHITE, 0.2f));

    const char *text = isTopView ? "BOARD VIEW" : "TOP VIEW";

    int fontSize = 20;
    int textWidth = MeasureText(text, fontSize);

    DrawText(
        text,
        x + (w - textWidth) / 2,
        y + (h - fontSize) / 2,
        fontSize,
        WHITE);

    switchCamBtn.render();
}