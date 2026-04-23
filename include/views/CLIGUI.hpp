#ifndef CLIGUI_HPP
#define CLIGUI_HPP

#include "views/IGUI.hpp"
#include "core/Game.hpp"
#include "models/Player/Player.hpp"
#include "models/Property/Property.hpp"
#include "models/BoardAndTiles/Tile.hpp"
#include "models/CardAndDeck/SkillCard.hpp"
#include "utils/data/LogEntry.hpp"

#include <string>
#include <vector>
#include <deque>

class CLIGUI : public IGUI {
private:
    bool exitRequested;
    std::deque<std::string> pendingCommands;
    bool awaitingInput;

public:
    CLIGUI();
    ~CLIGUI() override = default;

    void update() override;
    void display() override;
    bool shouldExit() const override;

    std::string getCommand() override;

    void loadMainMenu() override;
    void loadGameView() override;
    void loadFinishMenu() override;

    void showMessage(const std::string& message) override;
    void showConfirm(const std::string& question) override;
    void showInputPrompt(const std::string& prompt) override;

    void renderBoard(const Game& game) override;
    void renderPlayer(const Player& player) override;
    void renderProperty(const Property& property) override;
    void renderDice(int die1, int die2) override;
    void renderLog(const std::vector<LogEntry>& entries) override;
    void renderSkillHand(const std::vector<SkillCard*>& hand) override;
    void renderAuction(const Property& property, int currentBid, const Player* highBidder) override;
    void renderBankruptcy(const Player& player) override;
    void renderWinner(const Player& winner) override;
    void renderMovement(const std::string& playerName, int steps, const std::string& landedTileName) override;
};

#endif
