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
          }),
      rollDiceBtn(
          {200, 56},
          true,
          false,
          "NULL",
          []() {},
          []() {})
{
}

void GameHUDView::interactionCheck()
{
    rollDiceBtn.interactionCheck();
    switchCamBtn.interactionCheck();
}

std::string GameHUDView::catchCommand()
{
    std::string cmd;

    cmd = switchCamBtn.catchCommand();
    if (cmd != "NULL")
        return cmd;

    cmd = rollDiceBtn.catchCommand();
    if (cmd != "NULL")
        return cmd;

    return "NULL";
}

void GameHUDView::render()
{
    float margin = 20.0f;
    float spacing = 16.0f;

    float w = 200.0f;
    float h = 56.0f;

    float x = GetScreenWidth() - w - margin;

    float totalH = h * 2 + spacing;

    float startY = (GetScreenHeight() - totalH) / 2.0f;

    // SWITCH CAM (atas)
    float y1 = startY;

    switchCamBtn.movePosition({x + w / 2, y1 + h / 2});

    DrawRectangleRounded({x + 3, y1 + 4, w, h}, 0.5f, 8, Fade(BLACK, 0.25f));

    Color camColor = isTopView
                         ? Color{30, 110, 210, 255}
                         : Color{30, 170, 110, 255};

    DrawRectangleRounded({x, y1, w, h}, 0.5f, 8, camColor);

    const char *camText = isTopView ? "BOARD VIEW" : "TOP VIEW";

    int fontSize = 20;
    int textWidth = MeasureText(camText, fontSize);

    DrawText(camText,
             x + (w - textWidth) / 2,
             y1 + (h - fontSize) / 2,
             fontSize,
             WHITE);

    switchCamBtn.render();

    // ROLL DICE (bawah)
    float y2 = y1 + h + spacing;

    rollDiceBtn.movePosition({x + w / 2, y2 + h / 2});

    DrawRectangleRounded({x + 3, y2 + 4, w, h}, 0.5f, 8, Fade(BLACK, 0.25f));

    Color diceColor = Color{200, 140, 40, 255};

    DrawRectangleRounded({x, y2, w, h}, 0.5f, 8, diceColor);

    const char *diceText = "ROLL DICE";

    textWidth = MeasureText(diceText, fontSize);

    DrawText(diceText,
             x + (w - textWidth) / 2,
             y2 + (h - fontSize) / 2,
             fontSize,
             WHITE);

    rollDiceBtn.setGameCommand(
        "DISPLAY ROLL_DICE " + std::to_string(currentPlayerIdx));

    rollDiceBtn.render();
}

void GameHUDView::setCurrentPlayerIdx(int idx)
{
    currentPlayerIdx = idx;

    rollDiceBtn.setGameCommand(
        "DISPLAY ROLL_DICE " + std::to_string(currentPlayerIdx));
}