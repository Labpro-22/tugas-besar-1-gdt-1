#pragma once
#include "viewElement/MenuView.hpp"
#include "viewElement/Popup.hpp"
#include <set>
#include <stack>

class GUI {
    private:
        set<View2D*> views;
        stack<Popup*> popupStack;
        MenuView* menu;

        void unloadView(View2D* p);
    public:
        GUI();
        void loadMainMenu();
        void loadPopup(Popup* popup);
        void display();
        void enableAll();
        void disableAll();
        void update();
        string getCommand();
};