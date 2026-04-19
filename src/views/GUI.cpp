#include "views/GUI.hpp"

GUI::GUI() : menu(nullptr), exitRequested(false) {}

bool GUI::shouldExit() const {
    return exitRequested;
}

void GUI::loadGameView() {
    // TODO: muat view permainan utama
}

void GUI::loadFinishMenu() {
    // TODO: muat menu akhir permainan
}

void GUI::showMessage(const std::string& /*message*/) {
    // TODO: tampilkan popup pesan
}

void GUI::showConfirm(const std::string& /*question*/) {
    // TODO: tampilkan popup konfirmasi ya/tidak
}

void GUI::showInputPrompt(const std::string& /*prompt*/) {
    // TODO: tampilkan popup input teks
}

void GUI::renderBoard(const Game& /*game*/) {
    // TODO
}

void GUI::renderPlayer(const Player& /*player*/) {
    // TODO
}

void GUI::renderProperty(const Property& /*property*/) {
    // TODO
}

void GUI::renderDice(int /*die1*/, int /*die2*/) {
    // TODO
}

void GUI::renderLog(const std::vector<LogEntry>& /*entries*/) {
    // TODO
}

void GUI::renderSkillHand(const std::vector<SkillCard*>& /*hand*/) {
    // TODO
}

void GUI::renderAuction(const Property& /*property*/, int /*currentBid*/, const Player* /*highBidder*/) {
    // TODO
}

void GUI::renderBankruptcy(const Player& /*player*/) {
    // TODO
}

void GUI::renderWinner(const Player& /*winner*/) {
    // TODO
}

void GUI::unloadView(View2D* p) {
    if (p != nullptr) {
        views.erase(p);
        delete p;
    }
}


void GUI::loadMainMenu() {
    unloadView(menu);
    menu = new MainMenuView();
    views.insert(menu);
}

void GUI::enterGame() {
    cout<<"Entered Game"<<endl;
    (menu->getAnimation("START_GAME"))->start();
}

void GUI::loadPopup(Popup* popup) {
    disableAll();
    popupStack.push(popup);
    views.insert(popup);
}

string GUI::getCommand() {
    for (View2D* view : views) {
        string command = view->catchCommand();
        
        if (command != "NULL") {
            if ("DISPLAY " == command.substr(0, 8)) {
                
                stringstream ss(command);
                string item;
                vector<string> tokens;
                
                while(getline(ss, item, ' ')) {
                    tokens.push_back(item);
                }
                if (tokens[1] == "LOAD_CONFIRM_POPUP") { 
                    loadPopup(new LoadConfirmPopup(tokens[2])); 
                }
                return "NULL";
            }
            return command;
        }
    }
    return "NULL";
}

void GUI::enableAll() {
    for (View2D* view : views) {
        view->enable();
    }
}

void GUI::disableAll() {
    for (View2D* view : views) {
        view->disable();
    }
}


void GUI::update() {
    set<View2D*> closedViews;
    for (View2D* view : views) {
        if (view->closed()) {
            closedViews.insert(view);
        } else {
            view->interactionCheck();
        }
    }

    if (menu != nullptr) {
        if (menu->closed()) {
            menu = nullptr;
        }
    }

    if (!popupStack.empty()) {
        while(popupStack.top()->closed()) {            
            popupStack.pop();
            if (popupStack.empty()) {
                enableAll();
                break;
            } else {
                popupStack.top()->enable();
            }
        }
    }

    for (View2D* view : closedViews) {
        views.erase(view);
        delete view;
        view = nullptr;
    }
}

void GUI::display() {
    if (menu != nullptr) menu->render();
    stack<Popup*> temp = popupStack;
    while(!temp.empty()) {
        temp.top()->render();
        temp.pop();
    }
}