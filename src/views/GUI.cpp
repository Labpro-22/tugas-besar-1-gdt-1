#include "views/GUI.hpp"
#include "views/viewElement/GameHUDView.hpp"
#include "core/Game.hpp"
#include "models/CardAndDeck/LassoCard.hpp"

GUI::GUI(float fps, Board& board) : menu(nullptr), board(new BoardView(board)),
         debuggingEntry(nullptr), dice(nullptr), chancePile(nullptr), communityChestPile(nullptr), skillCard(nullptr),
         fps(fps), camManager(CameraManager()), exitRequested(false) {
    camManager.addCamera("BOARD_CAM", View3DCamera({this->board->getBoardSize()*1.1f, 10.0f, 0}, {0,0,0}, 45.0f));
    View3DCamera* boardCam = camManager.getCurrentCamera();
    boardCam->addMovement("ROTATE_INDEFINITE", 
        new CameraMovement(*camManager.getCurrentCamera(), 120, true, [boardCam, this](){
            boardCam->rotateAroundTarget(27*(1/this->fps), {0,1,0});
        }, [](){}));
    camManager.addCamera("TOP_VIEW", View3DCamera({-1.0f, this->board->getBoardSize()*1.25f, 0}, {0,0,0}, 45.0f));
    camManager.addCamera("ACTION_CAM", View3DCamera({-this->board->getBoardSize()*0.8f, this->board->getBoardSize()*0.6f, 0}, {0,0,0}, 45.0f));
    camManager.addCamera("TILE_CAM_1", View3DCamera({0,0,0}, {0,0,0}, 45.0f));
    camManager.addCamera("TILE_CAM_2", View3DCamera({0,0,0}, {0,0,0}, 45.0f));
}

bool GUI::shouldExit() const
{
    return exitRequested;
}

void GUI::requestExit()
{
    exitRequested = true;
}

void GUI::loadGameView()
{
    while (!popupStack.empty()) {
        delete popupStack.top();
        popupStack.pop();
    }

    for (auto v : views)
        delete v;
    views.clear();

    for (auto p : playerProfiles) delete p;
    playerProfiles.clear();

    for (auto p : players) delete p;
    players.clear();

    menu = nullptr;

    views.insert(new GameHUDView());
}

void GUI::loadFinishMenu()
{
    // TODO: muat menu akhir permainan
}

void GUI::showMessage(const std::string &message)
{
    loadPopup(new MessagePopup(message));
}

void GUI::showConfirm(const std::string & /*question*/)
{
    // TODO: tampilkan popup konfirmasi ya/tidak
}

void GUI::showInputPrompt(const std::string &prompt)
{
    loadPopup(new InputPopup(prompt));
}

void GUI::renderBoard(const Game &game)
{
    Board *b = game.getBoard();
    if (b == nullptr)
        return;

    if (board == nullptr)
    {
        board = new BoardView(*b);
    }
}

void GUI::renderPlayer(const Player & /*player*/)
{
    // TODO
}

void GUI::renderProperty(const Property & /*property*/)
{
    // TODO
}

void GUI::renderDice(int /*die1*/, int /*die2*/)
{
    // TODO
}

void GUI::renderLog(const std::vector<LogEntry> & /*entries*/)
{
    // TODO
}

void GUI::renderSkillHand(const std::vector<SkillCard *> & /*hand*/)
{
    // TODO
}

void GUI::renderAuction(const Property & /*property*/, int /*currentBid*/, const Player * /*highBidder*/)
{
    // TODO
}

void GUI::renderBankruptcy(const Player & /*player*/)
{
    // TODO
}

void GUI::renderWinner(const Player & /*winner*/)
{
    // TODO
}

void GUI::unloadView(View2D *p)
{
    if (p != nullptr)
    {
        views.erase(p);
        delete p;
    }
}

void GUI::loadMainMenu()
{
    unloadView(menu);
    menu = new MainMenuView();
    views.insert(menu);
}

void GUI::enterGame()
{
    cout << "Entered Game" << endl;
    (menu->getAnimation("START_GAME"))->start();
}

void GUI::loadPopup(Popup *popup)
{
    disableAll();
    popupStack.push(popup);
    views.insert(popup);
}

void GUI::loadPlayer(Player &player)
{
    Color color;
    switch (playerProfiles.size())
    {
    case 0:
        color = RED;
        break;
    case 1:
        color = BLUE;
        break;
    case 2:
        color = GREEN;
        break;
    case 3:
        color = YELLOW;
        break;
    default:
        color = LIGHTGRAY;
    }

    PlayerView *playerView = new PlayerView(player, board, color, &camManager);
    players.push_back(playerView);
    players.back()->setVisible(false);

    PlayerProfileView *profile = new PlayerProfileView();
    profile->setPlayer(&player);
    profile->setColor(color);

    profile->setHitboxDim({250, 80});

    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();

    float w = 250.0f;
    float h = 80.0f;
    float margin = 20.0f;

    int idx = playerProfiles.size();

    profile->setActive(true);

    playerProfiles.push_back(profile);
    views.insert(profile);
}

void GUI::loadDice(PlayerView* player) {
    dice = new DiceView(player, &camManager.getCamera("ACTION_CAM"));
    views.insert(dice->getThrowButton());
}

void GUI::loadCardPiles(CardDeck<Card>& chancePile, CardDeck<Card>& comChestPile) {
    Vector2 cardDim = (Vector2){7.6f, 4.275f}*(board->getBoardSize()/30.0f);
    Vector3 cardPos = {-cardDim.x/2 - board->getBoardSize()*0.015f, 0.015, -cardDim.x/2 - board->getBoardSize()*0.015f};
    this->chancePile = new CardPileView(chancePile, cardPos, cardDim);
    this->communityChestPile = new CardPileView(comChestPile, {cardPos.x*-1, cardPos.y, cardPos.z*-1}, cardDim*-1);
}

void GUI::loadSkillHand(Player& player, Card* incomingCard) {
    skillCard = new SkillHandView(player, camManager.getCurrentCamera(), incomingCard);
    views.insert(skillCard);
}

Command GUI::getCommand()
{
    if (!pendingCommand.isNull())
    {
        Command temp = pendingCommand;
        pendingCommand = Command::Null();
        return temp;
    }

    if (!popupStack.empty())
    {
        std::string raw = popupStack.top()->catchCommand();

        if (raw != "NULL")
        {
            Popup *p = popupStack.top();
            views.erase(p);
            delete p;
            popupStack.pop();

            std::stringstream ss(raw);
            std::string item;
            std::vector<std::string> tokens;

            while (getline(ss, item, ' '))
            {
                tokens.push_back(item);
            }

            if (tokens.empty())
                return {"NULL", {}};

            return {tokens[0],
                    std::vector<std::string>(tokens.begin() + 1, tokens.end())};
        }

        return {"NULL", {}};
    }

    for (View2D *view : views)
    {
        std::string raw = view->catchCommand();
        if (raw == "NULL")
            continue;

        std::stringstream ss(raw);
        std::string item;
        std::vector<std::string> tokens;

        while (getline(ss, item, ' '))
        {
            tokens.push_back(item);
        }

        if (tokens.empty())
            return {"NULL", {}};

        // HANDLE NEW GAME
        if (tokens[0] == "NEW_GAME")
        {
            auto popup = new LoadConfirmPopup("Masukkan path config:", "config/default");

            popup->setOnSubmit([this](const std::string &path)
                               { this->pendingCommand = Command("NEW_GAME", {path}); });

            loadPopup(popup);
            return Command::Null();
        }

        // HANDLE LOAD GAME
        if (tokens[0] == "LOAD_GAME")
        {
            auto popup = new LoadConfirmPopup("Masukkan save file:", "saves/save1");

            popup->setOnSubmit([this](const std::string &path)
                               { this->pendingCommand = Command("LOAD_GAME", {path}); });

            loadPopup(popup);
            return Command::Null();
        }

        // HANDLE INTERNAL GUI
        if (tokens[0] == "DISPLAY")
        {
            if (tokens[1] == "TOP_VIEW") {
                camManager.switchTo("TOP_VIEW", 1, [](){});
            } else if (tokens[1] == "ACTION_CAM") {
                camManager.switchTo("ACTION_CAM", 1, [](){});
            } else if (tokens[1] == "BOARD_CAM") {
                camManager.switchTo("BOARD_CAM", 1, [](){});
            } else if (tokens[1] == "ROLL_DICE") {
                camManager.switchTo("ACTION_CAM", 1, [this, tokens](){ loadDice(players[stoi(tokens[2])]); });
            } else if (tokens[1] == "THROW") {
                //call GameEngine buat dapetin angka dadu
                dice->initializeThrowDice(6, 6);
                dice->getThrowButton()->setActive(false);
            } else if (tokens[1] == "THROW_DONE") {
                dice->moveDiceOffScreen();
                PlayerView* movingPlayer = dice->getPlayer();
                int moveVal = dice->getMoveValue();
                camManager.switchTo(dice->getPlayer()->getPlayerCamKey(), 1, [this, movingPlayer, moveVal](){
                    movingPlayer->moveSpaces(moveVal);
                });
            } else if (tokens[1] == "DRAW") {
                camManager.switchTo("ACTION_CAM", 1, [this, tokens](){
                    if (tokens[2] == "CC") {
                        // reshuffle deck
                        communityChestPile->drawCard();
                    } else if (tokens[2] == "CH") {
                        // reshuffle deck
                        chancePile->drawCard();
                    }
                });
            } else if (tokens[1] == "HAND") {
                camManager.switchTo("ACTION_CAM", 1, [this, tokens](){
                    loadSkillHand(players[stoi(tokens[2])]->getPlayer(), new LassoCard);
                });
                
            } else if (tokens[1] == "BUILD") {
                board->getTileFromIdx(stoi(tokens[2]))->buildHouse();
            } else if (tokens[1] == "FOCUS_TILE") {
                if (camManager.getCurrentCamera() != &camManager.getCamera("TILE_CAM_1")) {
                    board->getTileFromIdx(stoi(tokens[2]))->setCamToTile(&camManager.getCamera("TILE_CAM_1"));
                    camManager.switchTo("TILE_CAM_1", 0.2, [](){});
                } else {
                    board->getTileFromIdx(stoi(tokens[2]))->setCamToTile(&camManager.getCamera("TILE_CAM_2"));
                    camManager.switchTo("TILE_CAM_2", 0.2, [](){});
                }
            } else if (tokens[1] == "SELL") {
                board->getTileFromIdx(stoi(tokens[2]))->sellHouse();
            }

            return {"NULL", {}};
        }

        // RETURN KE ENGINE
        return {tokens[0], std::vector<std::string>(tokens.begin() + 1, tokens.end())};
    }

    return {"NULL", {}};
}

void GUI::enableAll()
{
    for (View2D *view : views)
    {
        view->enable();
    }
}

void GUI::disableAll()
{
    for (View2D *view : views)
    {
        if (view != nullptr)
            view->disable();
    }
}

void GUI::updatePlayerProfilesLayout()
{
    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();

    float w = 250.0f;
    float h = 80.0f;
    float margin = 20.0f;

    for (int i = 0; i < playerProfiles.size(); i++)
    {
        Vector2 pos;

        switch (i)
        {
        case 0:
            pos = { w/2 + margin, h/2 + margin };
            break;
        case 1:
            pos = { screenW - w/2 - margin, h/2 + margin };
            break;
        case 2:
            pos = { w/2 + margin, screenH - h/2 - margin };
            break;
        case 3:
            pos = { screenW - w/2 - margin, screenH - h/2 - margin };
            break;
        }

        playerProfiles[i]->setPosition(pos);
    }
}

void GUI::update()
{
    camManager.updateCamMap();
    if (dice != nullptr) {
        dice->update();
        if (dice->isDone()) {
            delete dice;
            dice = nullptr;
        }
    }
    
  updatePlayerProfilesLayout();
    set<View2D*> closedViews;
    for (View2D* view : views) {
        if (view->closed()) {
            closedViews.insert(view);
        }
        else
        {
            view->interactionCheck();
        }
    }

    if (menu != nullptr)
    {
        if (menu->closed())
        {
            menu = nullptr;
        }
    }

    if (skillCard != nullptr) {
        if (skillCard->closed()) {
            skillCard = nullptr;
        }
    }

    if (!popupStack.empty())
    {
        while (popupStack.top()->closed())
        {
            popupStack.pop();
            if (popupStack.empty())
            {
                enableAll();
                break;
            }
            else
            {
                popupStack.top()->enable();
            }
        }
    }

    for (View2D *view : closedViews)
    {
        views.erase(view);
        delete view;
        view = nullptr;
    }
}

void GUI::display()
{
    BeginMode3D(camManager.mount());
        DrawGrid(40,1);
        board->render();
        for (PlayerView* player : players) {
            player->render();
        }
        if (chancePile != nullptr) chancePile->render();
        if (communityChestPile != nullptr) communityChestPile->render();
        if (dice != nullptr) dice->render();
    EndMode3D();
    for (View2D *view : views)
        view->render();
}

void GUI::loadDebuggingEntry()
{
    debuggingEntry = new Entry({800, 50}, "Enter Command", 30, "Orbitron", [this]()
                               { this->debuggingEntry->setGameCommand(this->debuggingEntry->getEntryText()); });
    debuggingEntry->movePosition({debuggingEntry->getRenderWidth(), debuggingEntry->getRenderHeight() / 2});
    views.insert(debuggingEntry);
}