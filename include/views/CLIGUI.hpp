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

class CellInfo {
private:
    std::string code;
    std::string colorTagStr;
    TileColor   color;
    std::string ownerTag;
    std::string building;
    std::string players;
    bool        jailCell;
    int         jailInmates;
    int         jailVisiting;
public:
    CellInfo()
        : color(TileColor::DEFAULT), jailCell(false),
          jailInmates(0), jailVisiting(0) {}

    void setCode(const std::string& v)      { code = v; }
    void setColorTag(const std::string& v)  { colorTagStr = v; }
    void setColor(TileColor v)              { color = v; }
    void setOwnerTag(const std::string& v)  { ownerTag = v; }
    void setBuilding(const std::string& v)  { building = v; }
    void appendPlayer(const std::string& v) { players += v; }
    void setJailCell(bool v)                { jailCell = v; }
    void setJailInmates(int v)              { jailInmates = v; }
    void setJailVisiting(int v)             { jailVisiting = v; }

    const std::string& getCode()      const { return code; }
    const std::string& getColorTag()  const { return colorTagStr; }
    TileColor          getColor()     const { return color; }
    const std::string& getOwnerTag()  const { return ownerTag; }
    const std::string& getBuilding()  const { return building; }
    const std::string& getPlayers()   const { return players; }
    bool               isJailCell()   const { return jailCell; }
    int                getJailInmates()  const { return jailInmates; }
    int                getJailVisiting() const { return jailVisiting; }
};

class CLIGUI : public IGUI {
private:
    bool exitRequested;
    std::deque<std::string> pendingCommands;
    bool awaitingInput;
    bool winnerBannerPrinted;

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
    void showException(int code, const std::string& msg) override;

    void renderBoard(const Game& game) override;
    void renderPlayer(const Player& player) override;
    void renderProperty(const Property& property) override;
    void renderOwnedProperties(const Player& player) override;
    void renderDice(int die1, int die2) override;
    void renderLog(const std::vector<LogEntry>& entries, const std::string& title) override;
    void renderSkillHand(const std::vector<SkillCard*>& hand) override;
    void renderAuction(const Property& property, int currentBid, const Player* highBidder) override;
    void renderBankruptcy(const Player& player) override;
    void renderWinner(const Player& winner) override;
    void renderMovement(const std::string& playerName, int steps, const std::string& landedTileName) override;
};

#endif
