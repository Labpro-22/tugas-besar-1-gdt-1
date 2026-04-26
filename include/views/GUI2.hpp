#pragma once

#include "views/IGUI.hpp"
#include "core/Game.hpp"
#include "models/Player/Player.hpp"
#include "models/Property/Property.hpp"
#include "models/CardAndDeck/SkillCard.hpp"
#include "utils/data/LogEntry.hpp"

#include "views/viewElement/Entry.hpp"
#include "views/viewElement/MenuView.hpp"
#include "views/viewElement/AuctionMenuView.hpp"
#include "views/viewElement/cards/CardPileView.hpp"
#include "views/viewElement/board/BoardView.hpp"
#include "views/viewElement/player/PlayerView.hpp"
#include "views/viewElement/player/DiceView.hpp"
#include "views/viewElement/Popup.hpp"
#include "views/viewElement/player/PlayerProfileView.hpp"
#include "views/animation/camera/CameraManager.hpp"
#include "views/animation/camera/CameraMovement.hpp"

#include <set>
#include <stack>
#include <vector>
#include <string>
#include <memory>
#include <queue>

class GUI : public IGUI
{
private:
    const Game *cachedGame = nullptr;
    bool showResume = false;
    Interactable resumeBtn;
    void setResumeVisible(bool v);

    // ── View layers ────────────────────────────────────────────────────────
    std::vector<std::unique_ptr<View2D>> views;
    std::stack<Popup *> popupStack;
    MenuView *menu;
    AuctionMenuView* auction;
    Entry *debuggingEntry;

    // ── 3D scene objects ───────────────────────────────────────────────────
    BoardView *board;
    DiceView *dice;
    std::vector<PlayerView *> players;
    std::vector<PlayerProfileView *> playerProfiles;
    CardPileView *chancePile;
    CardPileView *communityChestPile;

    // ── Camera & runtime ───────────────────────────────────────────────────
    CameraManager camManager;
    std::string pendingCommand;
    float fps;
    bool exitRequested;

    // ── View management ────────────────────────────────────────────────────
    void unloadView(View2D *v);
    void loadPopup(Popup *popup);
    void enableAll();
    void disableAll();
    void clearViews();
    void clearPopups();
    void clearPlayers();

    // ── Update helpers ─────────────────────────────────────────────────────
    void updateDice();
    void updateViews();
    void updatePopupStack();
    void updatePlayerProfilesLayout();

    // ── getCommand helpers ─────────────────────────────────────────────────
    bool hasPendingCommand() const;
    std::string consumePendingCommand();
    std::string pollPopup();
    std::string pollViews();
    void handleDisplayCommand(const std::vector<std::string> &tokens);

    // ── Popup delay after dice roll ─────────────────────────────────────────
    std::queue<Popup *> delayedPopupQueue;
    bool isDelayingPopupAfterDice;

    void waitForAnimToEnd2D(View2D* view);
    void waitForAnimToEnd3D(View3D* view);
    void waitForCameraMovementToEnd(View3DCamera* view);
    void waitFor(function<bool()> );
    void setHudDiceAnimationFinished(bool finished);
    void loadPopupNow(Popup *popup);
    void pumpPopupQueue();
    void updateDelayedPopups();

public:
    GUI(float fps, Board &board);
    ~GUI() override = default;

    // ── Lifecycle ──────────────────────────────────────────────────────────
    void update() override;
    void display() override;
    bool shouldExit() const override;

    // ── Input ──────────────────────────────────────────────────────────────
    std::string getCommand() override;

    // ── Navigasi view ──────────────────────────────────────────────────────
    void loadMainMenu() override;
    void loadGameView() override;
    void loadFinishMenu() override;

    // ── Popup / prompt ─────────────────────────────────────────────────────
    void showMessage(const std::string &message) override;
    void showConfirm(const std::string &question) override;
    void showInputPrompt(const std::string &prompt) override;
    void showException(int code, const std::string &msg) override;
    void waitForAnimationEnd() override;
    void showPauseMenu() override;

    // ── Render state game (kontrak sama persis dengan CLIGUI) ──────────────
    void renderBoard(const Game &game) override;
    void renderPlayer(const Player &player) override;
    void renderProperty(const Property &property) override;
    void renderOwnedProperties(const Player &player) override;
    void renderDice(int die1, int die2) override;
    void renderLog(const std::vector<LogEntry> &entries, const std::string &title) override;
    void renderSkillHand(const std::vector<SkillCard *> &hand) override;
    void renderAuctionStart(Property* property, Player *auctioner, Game* game) override;
    void renderAuctionTurn(Player* currentPlayer, bool forceBid) override;
    void renderAuctionUpdate(int currentBid, Player *highBidder) override;
    void renderAuctionEnd(Player* winner) override;
    void renderBankruptcy(const Player &player) override;
    void renderWinner(const Player &winner) override;
    void renderMovement(const std::string &playerName, int steps) override;
    void renderTeleport(const std::string &playerName, int targetIndex) override;

    // ── Setup khusus raylib (dipanggil engine saat inisialisasi game) ──────
    void loadPlayer(Player &player);
    void loadCardPiles(CardDeck<Card> &chancePile, CardDeck<Card> &comChestPile);
    void loadDice(PlayerView *player);
    void enterGame();

    // ── Testing ────────────────────────────────────────────────────────────
    void loadDebuggingEntry();
};