#pragma once
#include "utils/Command.hpp"
#include "views/IGUI.hpp"
#include "views/viewElement/Entry.hpp"
#include "views/viewElement/MenuView.hpp"
#include "views/viewElement/cards/CardView.hpp"
#include "views/viewElement/cards/CardPileView.hpp"
#include "views/viewElement/cards/SkillHandView.hpp"
#include "views/viewElement/board/BoardView.hpp"
#include "views/viewElement/player/PlayerView.hpp"
#include "views/viewElement/player/DiceView.hpp"
#include "views/viewElement/Popup.hpp"
#include "views/viewElement/player/PlayerProfileView.hpp"
#include "views/animation/ViewAnimation.hpp"
#include "views/animation/camera/CameraManager.hpp"
#include "views/animation/camera/CameraMovement.hpp"
#include <set>
#include <stack>

class GUI : public IGUI
{
private:
    set<View2D *> views;
    stack<Popup *> popupStack;
    MenuView *menu;
    Entry *debuggingEntry;
    BoardView *board;
    vector<PlayerView *> players;
    vector<PlayerProfileView *> playerProfiles;
    DiceView* dice;
    CardPileView* chancePile;
    CardPileView* communityChestPile;
    SkillHandView* skillCard;

    CameraManager camManager;

    Command pendingCommand;
    float fps;
    bool exitRequested;
    void unloadView(View2D *p);

public:
    GUI(float fps, Board &board);
    ~GUI() override = default;

    // Lifecycle
    void updatePlayerProfilesLayout();
    void update() override;
    void display() override;
    bool shouldExit() const override;
    void requestExit() override;

    // Input
    Command getCommand() override;

    // Navigasi view
    void loadMainMenu() override;
    void loadGameView() override;
    void loadFinishMenu() override;
    void enterGame();

    // Popup / prompt
    void showMessage(const std::string &message) override;
    void showConfirm(const std::string &question) override;
    void showInputPrompt(const std::string &prompt) override;

        // Utility khusus GUI raylib (bukan bagian IGUI)
        void loadPopup(Popup* popup);
        void loadPlayer(Player& player);
        void loadCardPiles(CardDeck<Card>& chancePile, CardDeck<Card>& comChestPile);
        void loadSkillHand(Player& player, Card* incomingCard);
        void loadDice(PlayerView* player);
        void enableAll();
        void disableAll();
    // Render state game
    void renderBoard(const Game &game) override;
    void renderPlayer(const Player &player) override;
    void renderProperty(const Property &property) override;
    void renderDice(int die1, int die2) override;
    void renderLog(const std::vector<LogEntry> &entries) override;
    void renderSkillHand(const std::vector<SkillCard *> &hand) override;
    void renderAuction(const Property &property, int currentBid, const Player *highBidder) override;
    void renderBankruptcy(const Player &player) override;
    void renderWinner(const Player &winner) override;


    // Testing
    void loadDebuggingEntry();
};
