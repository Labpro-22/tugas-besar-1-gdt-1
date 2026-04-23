#ifndef CLIGUI_HPP
#define CLIGUI_HPP

#include "views/IGUI.hpp"
#include "core/Game.hpp"
#include "models/Player/Player.hpp"
#include "models/Property/Property.hpp"
#include "models/BoardAndTiles/Tile.hpp"
#include "models/CardAndDeck/SkillCard.hpp"
#include "utils/data/LogEntry.hpp"

#include "models/BoardAndTiles/PropertyTile.hpp"
#include "models/Property/StreetProperty.hpp"
#include "models/BoardAndTiles/TileTypes.hpp"

#include <string>
#include <vector>
#include <deque>
#include <utility>

// Forward-declare the board cell helper class (defined in CLIGUI.cpp)
class CellInfo;

class CLIGUI : public IGUI {
private:
    bool exitRequested;
    std::deque<std::string> pendingCommands;
    bool awaitingInput;

    // ANSI color constants
    static const std::string ANSI_RESET;
    static const std::string FG_BROWN;
    static const std::string FG_LBLUE;
    static const std::string FG_PINK;
    static const std::string FG_ORANGE;
    static const std::string FG_RED;
    static const std::string FG_YELLOW;
    static const std::string FG_GREEN;
    static const std::string FG_DBLUE;
    static const std::string FG_GRAY;
    static const std::string FG_DEFAULT;

    static const int CELL_W;
    static const int CENTER_W;

    // Board rendering helpers
    static std::string colorPrefix(TileColor c);
    static std::string colorTag(TileColor c);
    static std::string buildingStr(BuildingState s);
    static CellInfo makeCellInfo(Tile* t, const Game& game);
    static std::string fitLeft(const std::string& s, int w);
    static std::string padCenter(const std::string& s, int w);
    static std::string paint(TileColor color, const std::string& text);
    static std::string centreLine(const std::string& s, int width);
    static std::string padRight(const std::string& s, int width);
    static std::string centredBlockLine(const std::string& s, int blockWidth, int totalWidth);
    static std::pair<std::string,std::string> cellContent(const CellInfo& ci);
    static void printHLine(const std::vector<CellInfo>& cells);
    static void printCellRow(const std::vector<CellInfo>& cells, bool closeBottom = false);
    static std::vector<std::string> buildCenterPanel(const Game& game);

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
    void renderOwnedProperties(const Player& player) override;
    void renderDice(int die1, int die2) override;
    void renderLog(const std::vector<LogEntry>& entries) override;
    void renderSkillHand(const std::vector<SkillCard*>& hand) override;
    void renderAuction(const Property& property, int currentBid, const Player* highBidder) override;
    void renderBankruptcy(const Player& player) override;
    void renderWinner(const Player& winner) override;
    void renderMovement(const std::string& playerName, int steps, const std::string& landedTileName) override;
};

#endif
