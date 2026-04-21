#pragma once
#include "views/IGUI.hpp"
#include "views/viewElement/MenuView.hpp"
#include "views/viewElement/Popup.hpp"
#include "views/animation/ViewAnimation.hpp"
#include "views/animation/camera/CameraManager.hpp"
#include "views/animation/camera/CameraMovement.hpp"
#include <set>
#include <stack>

class GUI : public IGUI {
    private:
        set<View2D*> views;
        stack<Popup*> popupStack;
        MenuView* menu;
        bool exitRequested;
        CameraManager camManager;
        float fps;
        void unloadView(View2D* p);
    public:
        GUI(float fps);
        ~GUI() override = default;

        // Lifecycle
        void update() override;
        void display() override;
        bool shouldExit() const override;

        // Input
        string getCommand() override;

        // Navigasi view
        void loadMainMenu() override;
        void loadGameView() override;
        void loadFinishMenu() override;
        void enterGame();

        // Popup / prompt
        void showMessage(const std::string& message) override;
        void showConfirm(const std::string& question) override;
        void showInputPrompt(const std::string& prompt) override;

        // Render state game
        void renderBoard(const Game& game) override;
        void renderPlayer(const Player& player) override;
        void renderProperty(const Property& property) override;
        void renderDice(int die1, int die2) override;
        void renderLog(const std::vector<LogEntry>& entries) override;
        void renderSkillHand(const std::vector<SkillCard*>& hand) override;
        void renderAuction(const Property& property, int currentBid, const Player* highBidder) override;
        void renderBankruptcy(const Player& player) override;
        void renderWinner(const Player& winner) override;

        // Utility khusus GUI raylib (bukan bagian IGUI)
        void loadPopup(Popup* popup);
        void enableAll();
        void disableAll();
};
