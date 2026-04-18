#include "views/GUI.hpp"

GUI::GUI() : menu(nullptr){}

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
    menu->render();
    stack<Popup*> temp = popupStack;
    while(!temp.empty()) {
        temp.top()->render();
        temp.pop();
    }
}