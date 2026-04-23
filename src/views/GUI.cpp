#include "views/GUI.hpp"
#include "core/Game.hpp"

GUI::GUI(float fps, Board &board) : menu(nullptr), board(new BoardView(board)),
                                    debuggingEntry(nullptr),
                                    fps(fps), camManager(CameraManager()), exitRequested(false)
{
    camManager.addCamera("BOARD_CAM", View3DCamera({this->board->getBoardSize() * 1.1f, 10.0f, 0}, {0, 0, 0}, 45.0f));
    View3DCamera *boardCam = camManager.getCurrentCamera();
    boardCam->addMovement("ROTATE_INDEFINITE",
                          new CameraMovement(*camManager.getCurrentCamera(), 120, true, [boardCam, this]()
                                             { boardCam->rotateAroundTarget(27 * (1 / this->fps), {0, 1, 0}); }, []() {}));
    camManager.addCamera("TOP_VIEW", View3DCamera({-1.0f, this->board->getBoardSize() * 1.25f, 0}, {0, 0, 0}, 45.0f));
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
    // TODO: muat view permainan utama
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
            if (tokens.size() >= 2 && tokens[1] == "SWITCH_TOP_VIEW")
            {
                camManager.switchTo("TOP_VIEW", 1);
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

void GUI::update()
{
    camManager.updateCamMap();
    set<View2D *> closedViews;
    for (View2D *view : views)
    {
        if (view->closed())
        {
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
    DrawGrid(40, 1);
    board->render();

    EndMode3D();
    if (menu != nullptr)
        menu->render();
    if (debuggingEntry != nullptr)
        debuggingEntry->render();
    stack<Popup *> temp = popupStack;
    while (!temp.empty())
    {
        temp.top()->render();
        temp.pop();
    }
}

void GUI::loadDebuggingEntry()
{
    debuggingEntry = new Entry({800, 50}, "Enter Command", 30, "Orbitron", [this]()
                               { this->debuggingEntry->setGameCommand(this->debuggingEntry->getEntryText()); });
    debuggingEntry->movePosition({debuggingEntry->getRenderWidth(), GetScreenHeight() - debuggingEntry->getRenderHeight() / 2});
    views.insert(debuggingEntry);
}