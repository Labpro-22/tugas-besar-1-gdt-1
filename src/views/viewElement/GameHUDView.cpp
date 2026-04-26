#include "views/viewElement/GameHUDView.hpp"
#include "core/Game.hpp"

namespace
{
    Color playerColor(size_t index)
    {
        switch (index)
        {
        case 0:
            return RED;
        case 1:
            return BLUE;
        case 2:
            return GREEN;
        case 3:
            return YELLOW;
        default:
            return LIGHTGRAY;
        }
    }
}

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
          []() {}),
      endTurnBtn(
          {200, 56},
          true,
          false,
          "AKHIRI_GILIRAN",
          []() {},
          [this]()
          {
              showEndTurnButton = false;
          }),
      pauseBtn(
          {50, 50},
          true,
          false,
          "PAUSE",
          []() {},
          []() {})
{
}

void GameHUDView::setGameModel(const Game *game)
{
    this->gameModel = game;
    inventoryPopup.reset();

    playerProfiles.clear();

    if (!gameModel)
        return;

    const auto &players = gameModel->getPlayers();

    for (size_t i = 0; i < players.size(); ++i)
    {
        PlayerProfileView profile;
        profile.setPlayer(players[i]);

        profile.setHitboxDim({250, 80});
        profile.setActive(true);

        profile.setColor(playerColor(i));
        playerProfiles.push_back(profile);
    }
}

void GameHUDView::updateProfileData()
{
    if (!gameModel)
    {
        showEndTurnButton = false;
        return;
    }

    rollDiceBtn.setGameCommand("LEMPAR_DADU");

    const Player *currentPlayer = gameModel->getCurrentPlayer();
    bool canEndTurnNow = false;

    if (currentPlayer != nullptr)
    {
        canEndTurnNow = currentPlayer->hasRolled() &&
                        !currentPlayer->hasPendingFestival() &&
                        currentPlayer->getStatus() == PlayerStatus::ACTIVE &&
                        currentPlayer->getConsecutiveDoubles() == 0;
    }

    showEndTurnButton = canEndTurnNow;
}

void GameHUDView::setLogs(const std::vector<std::string> &entries)
{
    logs = entries;

    if ((int)logs.size() > maxLogs)
        logs.erase(logs.begin(), logs.end() - maxLogs);
}

void GameHUDView::interactionCheck()
{
    if (inventoryPopup)
    {
        inventoryPopup->interactionCheck();
        if (inventoryPopup->closed())
        {
            inventoryPopup.reset();
        }
        return;
    }

    rollDiceBtn.interactionCheck();
    switchCamBtn.interactionCheck();
    pauseBtn.interactionCheck();

    if (showEndTurnButton)
    {
        endTurnBtn.interactionCheck();
    }

    for (auto &p : playerProfiles)
    {
        p.interactionCheck();

        std::string cmd = p.catchCommand();
        if (cmd != "NULL")
        {
            Player *clickedPlayer = p.getPlayer();
            Player *currentPlayer = gameModel ? gameModel->getCurrentPlayer() : nullptr;

            if (clickedPlayer != nullptr && clickedPlayer == currentPlayer)
            {
                inventoryPopup = std::make_unique<PlayerInventoryPopup>(clickedPlayer);
                inventoryPopup->enable();
                break;
            }
        }
    }
}

std::string GameHUDView::catchCommand()
{
    if (inventoryPopup)
    {
        std::string popupCmd = inventoryPopup->catchCommand();
        if (popupCmd != "NULL")
            return popupCmd;
    }

    std::string cmd;

    cmd = switchCamBtn.catchCommand();
    if (cmd != "NULL")
        return cmd;

    cmd = rollDiceBtn.catchCommand();
    if (cmd != "NULL")
        return cmd;

    cmd = endTurnBtn.catchCommand();
    if (cmd != "NULL")
        return cmd;

    cmd = pauseBtn.catchCommand();
    if (cmd != "NULL")
        return cmd;

    for (auto &p : playerProfiles)
    {
        cmd = p.catchCommand();
        if (cmd != "NULL")
            return cmd;
    }

    return "NULL";
}

void GameHUDView::render()
{
    updateProfileData();
    float wProfile = 250.0f;
    float hProfile = 80.0f;
    float margin = 20.0f;

    const Player *currentPlayer = gameModel ? gameModel->getCurrentPlayer() : nullptr;
    const std::vector<Player *> *players = gameModel ? &gameModel->getPlayers() : nullptr;

    Vector2 corners[4] = {
        {wProfile / 2 + margin, hProfile / 2 + margin},                                       // TL
        {GetScreenWidth() - wProfile / 2 - margin, hProfile / 2 + margin},                    // TR
        {wProfile / 2 + margin, GetScreenHeight() - hProfile / 2 - margin},                   // BL
        {GetScreenWidth() - wProfile / 2 - margin, GetScreenHeight() - hProfile / 2 - margin} // BR
    };

    for (size_t i = 0; i < playerProfiles.size() && i < 4; ++i)
    {
        playerProfiles[i].movePosition(corners[i]);
        playerProfiles[i].render();

        if (currentPlayer != nullptr && players != nullptr && i < players->size() && (*players)[i] == currentPlayer)
        {
            Rectangle rect = playerProfiles[i].getHitbox();

            DrawRectangleLinesEx(
                {rect.x - 3, rect.y - 3, rect.width + 6, rect.height + 6},
                4,
                Color{255, 215, 0, 255});
        }
    }

    float spacing = 16.0f;

    float w = 200.0f;
    float h = 56.0f;

    float x = GetScreenWidth() - w - margin;
    float totalH = h * (showEndTurnButton ? 3 : 2) + spacing * (showEndTurnButton ? 2 : 1);
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

    rollDiceBtn.render();

    float logW = GetScreenWidth() * 0.7f;
    float logH = 160;
    float logX = (GetScreenWidth() - logW) / 2.0f;
    float logY = GetScreenHeight() - logH - 20;

    float headerH = 30;

    // background
    DrawRectangle(logX, logY, logW, logH, Color{30, 30, 30, 200});

    // header
    DrawRectangle(logX, logY, logW, headerH, Color{50, 50, 50, 220});

    // title
    DrawText("LOG", logX + 12, logY + 6, 18, WHITE);

    // text setup
    Vector2 textPos = {logX + 12, logY + headerH + 8};
    Vector2 textArea = {logW - 24, logH - headerH - 12};

    std::string fullText;
    for (const auto &log : logs)
    {
        fullText += log + "\n";
    }

    // clip hanya area isi
    BeginScissorMode(
        (int)logX,
        (int)(logY + headerH),
        (int)logW,
        (int)(logH - headerH));

    drawTextWrappedBox(
        fontMap["Orbitron"],
        fullText,
        textPos,
        16,
        1,
        LIGHTGRAY,
        textArea);

    EndScissorMode();

    // ===== PAUSE BUTTON =====
    Rectangle profileBox = playerProfiles[0].getHitbox();

    float size = 44;
    float px = profileBox.x + profileBox.width + 10;
    float py = profileBox.y;

    pauseBtn.movePosition({px + size / 2, py + size / 2});

    DrawRectangleRounded({px, py, size, size}, 0.3f, 6, Color{50, 50, 50, 200});

    float barW = 6;
    float barH = 20;
    float gap = 4;

    DrawRectangle(px + size / 2 - gap - barW, py + size / 2 - barH / 2, barW, barH, WHITE);
    DrawRectangle(px + size / 2 + gap, py + size / 2 - barH / 2, barW, barH, WHITE);

    pauseBtn.render();

    if (showEndTurnButton)
    {
        float y3 = y2 + h + spacing;
        endTurnBtn.movePosition({x + w / 2, y3 + h / 2});
        DrawRectangleRounded({x + 3, y3 + 4, w, h}, 0.5f, 8, Fade(BLACK, 0.25f));

        Color endTurnColor = Color{160, 55, 55, 255};
        DrawRectangleRounded({x, y3, w, h}, 0.5f, 8, endTurnColor);

        const char *endText = "END TURN";
        textWidth = MeasureText(endText, fontSize);

        DrawText(endText,
                 x + (w - textWidth) / 2,
                 y3 + (h - fontSize) / 2,
                 fontSize,
                 WHITE);

        endTurnBtn.render();
    }

    if (inventoryPopup)
    {
        inventoryPopup->render();
    }
}