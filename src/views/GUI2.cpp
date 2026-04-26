#include "views/GUI2.hpp"
#include "views/viewElement/GameHUDView.hpp"
#include "core/Game.hpp"

#include <sstream>
#include <iostream>

namespace
{

    std::vector<std::string> tokenize(const std::string &raw)
    {
        std::vector<std::string> tokens;
        std::stringstream ss(raw);
        std::string word;
        while (std::getline(ss, word, ' '))
            if (!word.empty())
                tokens.push_back(word);
        return tokens;
    }

    std::string join(const std::vector<std::string> &tokens, size_t from = 0)
    {
        std::string result;
        for (size_t i = from; i < tokens.size(); ++i)
        {
            if (i > from)
                result += ' ';
            result += tokens[i];
        }
        return result;
    }

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

} // namespace

// ═══════════════════════════════════════════════════════════════════════════
// Constructor
// ═══════════════════════════════════════════════════════════════════════════

GUI::GUI(float fps, Board &board)
    : menu(nullptr),
      board(new BoardView(board)),
      debuggingEntry(nullptr),
      dice(nullptr),
      chancePile(nullptr),
      communityChestPile(nullptr),
      camManager(CameraManager()),
      pendingCommand("NULL"),
      fps(fps),
            exitRequested(false)
{
    const float boardSize = this->board->getBoardSize();

    // Kamera utama — berputar mengelilingi board
    camManager.addCamera("BOARD_CAM",
                         View3DCamera({boardSize * 1.1f, 10.0f, 0.0f}, {0, 0, 0}, 45.0f));

    View3DCamera *boardCam = camManager.getCurrentCamera();
    boardCam->addMovement("ROTATE_INDEFINITE",
                          new CameraMovement(*boardCam, 120, true, [boardCam, this]()
                                             { boardCam->rotateAroundTarget(27.0f * (1.0f / this->fps), {0, 1, 0}); }, []() {}));

    // Kamera top-down untuk overview
    camManager.addCamera("TOP_VIEW",
                         View3DCamera({-1.0f, boardSize * 1.25f, 0.0f}, {0, 0, 0}, 45.0f));

    // Kamera aksi untuk giliran pemain
    camManager.addCamera("ACTION_CAM",
                         View3DCamera({-boardSize * 0.8f, boardSize * 0.6f, 0.0f}, {0, 0, 0}, 45.0f));
}

// ═══════════════════════════════════════════════════════════════════════════
// Lifecycle
// ═══════════════════════════════════════════════════════════════════════════

bool GUI::shouldExit() const
{
    return exitRequested;
}

void GUI::update()
{
    if (IsKeyPressed(KEY_F11))
    {
        ToggleFullscreen();
    }

    if (WindowShouldClose())
        exitRequested = true;

    camManager.updateCamMap();

    updateDice();
    updatePlayerProfilesLayout();
    updateViews();
    updatePopupStack();
}

void GUI::display()
{
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(camManager.mount());
    DrawGrid(40, 1);
    board->render();
    for (PlayerView *p : players)
        p->render();
    if (chancePile)
        chancePile->render();
    if (communityChestPile)
        communityChestPile->render();
    if (dice)
        dice->render();
    EndMode3D();

    for (auto &view : views)
        view->render();

    EndDrawing();
}

// ═══════════════════════════════════════════════════════════════════════════
// Navigasi view
// ═══════════════════════════════════════════════════════════════════════════

void GUI::loadMainMenu()
{
    pendingCommand = "NULL";
    unloadView(menu);

    views.push_back(std::make_unique<MainMenuView>());
    menu = static_cast<MainMenuView *>(views.back().get());
}

void GUI::loadGameView()
{
    clearPopups();
    clearViews();
    clearPlayers();
    menu = nullptr;

    auto hud = std::make_unique<GameHUDView>();

    if (this->cachedGame != nullptr)
        hud->setGameModel(this->cachedGame);

    views.push_back(std::move(hud));
}

void GUI::loadFinishMenu()
{
    // TODO: tampilkan layar akhir permainan
}

void GUI::enterGame()
{
    if (menu != nullptr)
        menu->getAnimation("START_GAME")->start();
}

// ═══════════════════════════════════════════════════════════════════════════
// Popup / prompt
// ═══════════════════════════════════════════════════════════════════════════

void GUI::showMessage(const std::string &message)
{
    loadPopup(new MessagePopup(message));
}

void GUI::showConfirm(const std::string & /*question*/)
{
    // TODO: popup konfirmasi ya/tidak
}

void GUI::showInputPrompt(const std::string &prompt)
{
    loadPopup(new InputPopup(prompt));
}

void GUI::showException(int code, const std::string &msg)
{
    loadPopup(new ExceptionPopup(code, msg));
}

// ═══════════════════════════════════════════════════════════════════════════
// Input — getCommand
// Prioritas: pendingCommand → popup aktif → views
// ═══════════════════════════════════════════════════════════════════════════

std::string GUI::getCommand()
{
    if (hasPendingCommand())
        return consumePendingCommand();

    if (!popupStack.empty())
        return pollPopup();

    return pollViews();
}

// ═══════════════════════════════════════════════════════════════════════════
// Render — semua stub; diisi saat fitur GUI siap
// ═══════════════════════════════════════════════════════════════════════════

void GUI::renderBoard(const Game &game)
{
    this->cachedGame = &game;

    for (auto &view : views)
    {
        if (auto *hud = dynamic_cast<GameHUDView *>(view.get()))
            hud->setGameModel(this->cachedGame);
    }

    if (board == nullptr)
    {
        Board *b = game.getBoard();
        if (b != nullptr)
            board = new BoardView(*b);
    }
}

void GUI::renderPlayer(const Player &player)
{
    for (auto *pv : players)
    {
        if (&(pv->getPlayer()) == &player)
            return;
    }

    PlayerView *view = new PlayerView(
        const_cast<Player &>(player),
        board,
        playerColor(players.size()),
        &camManager);

    players.push_back(view);
}

void GUI::renderProperty(const Property & /*property*/) { /* TODO */ }
void GUI::renderOwnedProperties(const Player & /*player*/) { /* TODO */ }
void GUI::renderDice(int d1, int d2)
{
    if (dice != nullptr)
        delete dice;

    for (auto &view : views)
    {
        if (auto *hud = dynamic_cast<GameHUDView *>(view.get()))
            hud->setDiceAnimationFinished(false);
    }

    int idx = cachedGame->getCurrentTurnIndex();
    dice = new DiceView(players[idx], &camManager.getCamera("ACTION_CAM"));
    dice->initializeThrowDice(d1, d2);
}
void GUI::renderSkillHand(const std::vector<SkillCard *> & /*hand*/) { /* TODO */ }
void GUI::renderBankruptcy(const Player & /*player*/) { /* TODO */ }
void GUI::renderWinner(const Player & /*winner*/) { /* TODO */ }

void GUI::renderLog(const std::vector<LogEntry> & /*entries*/, const std::string & /*title*/)
{
    // TODO: tampilkan log di panel HUD
}

void GUI::renderAuction(const Property & /*property*/, int /*currentBid*/, const Player * /*highBidder*/)
{
    // TODO: tampilkan popup lelang
}

void GUI::renderMovement(const std::string & /*playerName*/, int /*steps*/, const std::string & /*landedTileName*/)
{
    // TODO: animasi gerak pemain sudah ditangani PlayerView::moveSpaces();
    //       fungsi ini bisa dipakai untuk overlay teks atau kamera tracking
}

// ═══════════════════════════════════════════════════════════════════════════
// Setup khusus raylib
// ═══════════════════════════════════════════════════════════════════════════

void GUI::loadPlayer(Player &player)
{
    Color color = playerColor(playerProfiles.size());

    auto profile = std::make_unique<PlayerProfileView>();
    profile->setPlayer(&player);
    profile->setColor(color);
    profile->setHitboxDim({250, 80});
    profile->setActive(true);

    views.push_back(std::move(profile));
    playerProfiles.push_back(static_cast<PlayerProfileView *>(views.back().get()));
}

void GUI::loadDice(PlayerView *player)
{
    dice = new DiceView(player, &camManager.getCamera("ACTION_CAM"));
    if (dice && dice->getThrowButton())
        dice->getThrowButton()->render();
}

void GUI::loadCardPiles(CardDeck<Card> &chanceDeck, CardDeck<Card> &comChestDeck)
{
    const float boardSize = board->getBoardSize();
    const Vector2 cardDim = (Vector2){7.6f, 4.275f} * (boardSize / 30.0f);
    const Vector3 cardPos = {
        -cardDim.x / 2.0f - boardSize * 0.015f,
        0.015f,
        -cardDim.x / 2.0f - boardSize * 0.015f};

    chancePile = new CardPileView(chanceDeck, cardPos, cardDim);
    communityChestPile = new CardPileView(comChestDeck,
                                          {-cardPos.x, cardPos.y, -cardPos.z},
                                          {-cardDim.x, -cardDim.y});
}

// ═══════════════════════════════════════════════════════════════════════════
// Testing
// ═══════════════════════════════════════════════════════════════════════════

void GUI::loadDebuggingEntry()
{
    Entry *raw = nullptr;

    auto entry = std::make_unique<Entry>(
        Vector2{800, 50}, "Enter Command", 30, "Orbitron",
        [&raw]()
        {
            raw->setGameCommand(raw->getEntryText());
        });

    raw = entry.get();

    debuggingEntry = raw;

    raw->movePosition({raw->getRenderWidth(),
                       raw->getRenderHeight() / 2.0f});

    views.push_back(std::move(entry));
}

// ═══════════════════════════════════════════════════════════════════════════
// Private — helpers internal
// ═══════════════════════════════════════════════════════════════════════════

// ── View management ────────────────────────────────────────────────────────

void GUI::unloadView(View2D *v)
{
    views.erase(
        std::remove_if(views.begin(), views.end(),
                       [v](const std::unique_ptr<View2D> &ptr)
                       {
                           return ptr.get() == v;
                       }),
        views.end());
}

void GUI::loadPopup(Popup *popup)
{
    if (popup == nullptr)
        return;

    if (!popupStack.empty() || dice != nullptr || !delayedPopupQueue.empty())
    {
        delayedPopupQueue.push(popup);
        return;
    }

    disableAll();
    views.push_back(std::unique_ptr<View2D>(popup));
    popupStack.push(popup);
}

void GUI::enableAll()
{
    for (auto &v : views)
        v->enable();
}

void GUI::disableAll()
{
    for (auto &v : views)
        if (v)
            v->disable();
}

void GUI::clearViews()
{
    views.clear();
}

void GUI::clearPopups()
{
    while (!popupStack.empty())
        popupStack.pop();
    while (!delayedPopupQueue.empty())
    {
        delete delayedPopupQueue.front();
        delayedPopupQueue.pop();
    }
}

void GUI::clearPlayers()
{
    for (PlayerView *p : players)
        delete p;
    for (PlayerProfileView *p : playerProfiles)
        delete p;
    players.clear();
    playerProfiles.clear();
}

// ── Update helpers ─────────────────────────────────────────────────────────

void GUI::updateDice()
{
    if (dice == nullptr)
        return;

    std::string diceCmd = dice->getThrowButton()->catchCommand();
    if (diceCmd != "NULL")
    {
        auto tokens = tokenize(diceCmd);
        if (!tokens.empty() && tokens[0] == "DISPLAY")
            handleDisplayCommand(tokens);
    }

    dice->update();
    if (dice->isDone())
    {
        delete dice;
        dice = nullptr;

        for (auto &view : views)
        {
            if (auto *hud = dynamic_cast<GameHUDView *>(view.get()))
                hud->setDiceAnimationFinished(true);
        }
    }
}

void GUI::updateViews()
{
    views.erase(
        std::remove_if(views.begin(), views.end(),
                       [](const std::unique_ptr<View2D> &v)
                       {
                           if (v->closed())
                               return true;
                           v->interactionCheck();
                           return false;
                       }),
        views.end());

    if (menu != nullptr && menu->closed())
        menu = nullptr;
}

void GUI::updatePopupStack()
{
    while (!popupStack.empty() && popupStack.top()->closed())
    {
        popupStack.pop();
        if (popupStack.empty())
            enableAll();
        else
            popupStack.top()->enable();
    }

    if (dice == nullptr && popupStack.empty() && !delayedPopupQueue.empty())
    {
        Popup *popup = delayedPopupQueue.front();
        delayedPopupQueue.pop();
        disableAll();
        views.push_back(std::unique_ptr<View2D>(popup));
        popupStack.push(popup);
    }
}

void GUI::updatePlayerProfilesLayout()
{
    const float screenW = static_cast<float>(GetScreenWidth());
    const float screenH = static_cast<float>(GetScreenHeight());
    const float w = 250.0f, h = 80.0f, margin = 20.0f;

    const Vector2 corners[4] = {
        {w / 2.0f + margin, h / 2.0f + margin},
        {screenW - w / 2.0f - margin, h / 2.0f + margin},
        {w / 2.0f + margin, screenH - h / 2.0f - margin},
        {screenW - w / 2.0f - margin, screenH - h / 2.0f - margin},
    };

    for (size_t i = 0; i < playerProfiles.size() && i < 4; ++i)
        playerProfiles[i]->setPosition(corners[i]);
}

// ── getCommand helpers ─────────────────────────────────────────────────────

bool GUI::hasPendingCommand() const
{
    return pendingCommand != "NULL";
}

std::string GUI::consumePendingCommand()
{
    std::string cmd = pendingCommand;
    pendingCommand = "NULL";
    return cmd;
}

std::string GUI::pollPopup()
{
    if (popupStack.empty())
        return "NULL";

    std::string raw = popupStack.top()->catchCommand();
    if (raw == "NULL")
        return "NULL";

    View2D *p = popupStack.top();
    popupStack.pop();

    unloadView(p);

    if (popupStack.empty())
        enableAll();
    else
        popupStack.top()->enable();

    return raw;
}

std::string GUI::pollViews()
{
    for (auto &view : views)
    {
        std::string raw = view->catchCommand();
        if (raw == "NULL")
            continue;

        auto tokens = tokenize(raw);
        if (tokens.empty())
            continue;

        const std::string &cmd = tokens[0];

        if (cmd == "NEW_GAME")
        {
            auto popup = new LoadConfirmPopup("Masukkan path config:", "config/default");
            popup->setOnSubmit([this](const std::string &path)
                               { pendingCommand = "NEW_GAME " + path; });
            loadPopup(popup);
            return "NULL";
        }

        if (cmd == "LOAD_GAME")
        {
            auto popup = new LoadConfirmPopup("Masukkan save file:", "saves/save1");
            popup->setOnSubmit([this](const std::string &path)
                               { pendingCommand = "LOAD_GAME " + path; });
            loadPopup(popup);
            return "NULL";
        }

        if (cmd == "DISPLAY")
        {
            handleDisplayCommand(tokens);
            return "NULL";
        }

        // Teruskan mentah ke engine
        return raw;
    }

    return "NULL";
}

void GUI::handleDisplayCommand(const std::vector<std::string> &tokens)
{
    if (tokens.size() < 2)
        return;
    const std::string &sub = tokens[1];

    if (sub == "TOP_VIEW")
        camManager.switchTo("TOP_VIEW", 1, []() {});
    else if (sub == "ACTION_CAM")
        camManager.switchTo("ACTION_CAM", 1, []() {});
    else if (sub == "BOARD_CAM")
        camManager.switchTo("BOARD_CAM", 1, []() {});

    else if (sub == "ROLL_DICE" && tokens.size() >= 3)
    {
        int idx = std::stoi(tokens[2]);
        camManager.switchTo("ACTION_CAM", 1, [this, idx]()
                            { loadDice(players[idx]); });
    }
    else if (sub == "THROW" && dice != nullptr)
    {
        dice->initializeThrowDice(6, 6);
        dice->getThrowButton()->setActive(false);
    }
    else if (sub == "THROW_DONE" && dice != nullptr)
    {
        dice->moveDiceOffScreen();
        PlayerView *movingPlayer = dice->getPlayer();
        int moveVal = dice->getMoveValue();
        std::string camKey = movingPlayer->getPlayerCamKey();

        camManager.switchTo(camKey, 1, [movingPlayer, moveVal]()
                            { movingPlayer->moveSpaces(moveVal); });
    }
    else if (sub == "DRAW" && tokens.size() >= 3)
    {
        if (tokens[2] == "CC")
            communityChestPile->drawCard();
        else if (tokens[2] == "CH")
            chancePile->drawCard();
    }
}