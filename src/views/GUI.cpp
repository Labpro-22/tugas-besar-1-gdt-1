#include "views/GUI.hpp"

GUI::GUI(float fps, Board& board) : menu(nullptr), board(new BoardView(board)),
         debuggingEntry(nullptr),
         fps(fps), camManager(CameraManager()), exitRequested(false) {
    camManager.addCamera("BOARD_CAM", View3DCamera({this->board->getBoardSize()*1.1f, 10.0f, 0}, {0,0,0}, 45.0f));
    View3DCamera* boardCam = camManager.getCurrentCamera();
    boardCam->addMovement("ROTATE_INDEFINITE", 
        new CameraMovement(*camManager.getCurrentCamera(), 120, true, [boardCam, this](){
            boardCam->rotateAroundTarget(27*(1/this->fps), {0,1,0});
        }, [](){}));
    camManager.addCamera("TOP_VIEW", View3DCamera({-1.0f, this->board->getBoardSize()*1.25f, 0}, {0,0,0}, 45.0f));
}

bool GUI::shouldExit() const {
    return exitRequested;
}

void GUI::loadGameView()
{
    // TODO: muat view permainan utama
}

void GUI::loadFinishMenu()
{
    // TODO: muat menu akhir permainan
}

void GUI::showMessage(const std::string & /*message*/)
{
    // TODO: tampilkan popup pesan
}

void GUI::showConfirm(const std::string & /*question*/)
{
    // TODO: tampilkan popup konfirmasi ya/tidak
}

void GUI::showInputPrompt(const std::string & /*prompt*/)
{
    // TODO: tampilkan popup input teks
}

void GUI::renderBoard(const Game & /*game*/)
{
    // TODO
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

void GUI::loadPlayer(Player& player) {
    Color playerColor;
    switch(players.size()) {
        case 0: playerColor = RED; break;
        case 1: playerColor = BLUE; break;
        case 2: playerColor = GREEN; break;
        case 3: playerColor = YELLOW; break;
        default: playerColor = LIGHTGRAY;
    }
    players.push_back(new PlayerView(player, board, playerColor, &camManager));
}

string GUI::getCommand() {
    for (View2D* view : views) {
        string command = view->catchCommand();

        if (command != "NULL")
        {
            if ("DISPLAY " == command.substr(0, 8))
            {

                stringstream ss(command);
                string item;
                vector<string> tokens;

                while (getline(ss, item, ' '))
                {
                    tokens.push_back(item);
                }
                if (tokens[1] == "LOAD_CONFIRM_POPUP") { 
                    loadPopup(new LoadConfirmPopup(tokens[2])); 
                } else if (tokens[1] == "TOP_VIEW") {
                    camManager.switchTo("TOP_VIEW", 1, [](){});
                } else if (tokens[1] == "MOVE_PLAYER") {
                    camManager.switchTo(players[stoi(tokens[2])]->getPlayerCamKey(), 1, [this, tokens](){
                        players[stoi(tokens[2])]->moveToTile(*board->getTileFromIdx(stoi(tokens[3])));
                    });
                } else if (tokens[1] == "BOARD_CAM") {
                    camManager.switchTo("BOARD_CAM", 1, [](){});
                }
                else if (tokens.size() >= 4 && tokens[1] == "EXCEPTION_POPUP")
                {
                    int errorCode = stoi(tokens[2]);

                    string errorMessage = "";
                    for (size_t i = 3; i < tokens.size(); ++i)
                    {
                        errorMessage += tokens[i];
                        if (i < tokens.size() - 1)
                        {
                            errorMessage += " ";
                        }
                    }

                    loadPopup(new ExceptionPopup(errorCode, errorMessage));
                }
                return "NULL";
            }
            return command;
        }
    }
    return "NULL";
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
        view->disable();
    }
}


void GUI::update() {
    camManager.updateCamMap();
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