#include "views/CLIGUI.hpp"
#include "models/BoardAndTiles/Board.hpp"
#include "models/BoardAndTiles/PropertyTile.hpp"
#include "models/BoardAndTiles/SpecialTile/JailTile.hpp"
#include "models/Property/StreetProperty.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>

// ── ANSI helpers ─────────────────────────────────────────────────────────────

#define RESET   "\033[0m"
#define FG_BROWN     "\033[38;5;130m"
#define FG_LBLUE     "\033[38;5;117m"
#define FG_PINK      "\033[38;5;201m"
#define FG_ORANGE    "\033[38;5;208m"
#define FG_RED       "\033[38;5;196m"
#define FG_YELLOW    "\033[38;5;220m"
#define FG_GREEN     "\033[38;5;76m"
#define FG_DBLUE     "\033[38;5;27m"
#define FG_GRAY      "\033[38;5;250m"
#define FG_DEFAULT   "\033[39m"

static std::string colorPrefix(TileColor c) {
    switch (c) {
        case TileColor::COKLAT:     return FG_BROWN;
        case TileColor::BIRU_MUDA:  return FG_LBLUE;
        case TileColor::MERAH_MUDA: return FG_PINK;
        case TileColor::ORANYE:     return FG_ORANGE;
        case TileColor::MERAH:      return FG_RED;
        case TileColor::KUNING:     return FG_YELLOW;
        case TileColor::HIJAU:      return FG_GREEN;
        case TileColor::BIRU_TUA:   return FG_DBLUE;
        case TileColor::ABU_ABU:    return FG_GRAY;
        default:                    return FG_DEFAULT;
    }
}

static std::string colorTag(TileColor c) {
    switch (c) {
        case TileColor::COKLAT:     return "CK";
        case TileColor::BIRU_MUDA:  return "BM";
        case TileColor::MERAH_MUDA: return "PK";
        case TileColor::ORANYE:     return "OR";
        case TileColor::MERAH:      return "MR";
        case TileColor::KUNING:     return "KN";
        case TileColor::HIJAU:      return "HJ";
        case TileColor::BIRU_TUA:   return "BT";
        case TileColor::ABU_ABU:    return "AB";
        default:                    return "DF";
    }
}

// Board cell helpers 
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

static std::string buildingStr(BuildingState s) {
    switch (s) {
        case BuildingState::HOUSE_1: return "^";
        case BuildingState::HOUSE_2: return "^^";
        case BuildingState::HOUSE_3: return "^^^";
        case BuildingState::HOUSE_4: return "^^^^";
        case BuildingState::HOTEL:   return "*";
        default:                     return "";
    }
}

static CellInfo makeCellInfo(Tile* t, const Game& game) {
    CellInfo ci;
    ci.setCode(t->getCode());
    ci.setColor(t->getColor());
    ci.setColorTag(colorTag(t->getColor()));

    // ownership & building
    if (auto* pt = dynamic_cast<PropertyTile*>(t)) {
        if (Property* p = pt->getProperty()) {
            if (p->getOwner()) {
                const auto& allPlayers = game.getPlayers();
                for (int i = 0; i < (int)allPlayers.size(); ++i) {
                    if (allPlayers[i] == p->getOwner()) {
                        ci.setOwnerTag("P" + std::to_string(i + 1));
                        break;
                    }
                }
            }
            if (auto* sp = dynamic_cast<StreetProperty*>(p)) {
                ci.setBuilding(buildingStr(sp->getBuildingState()));
            }
        }
    }

    // jail special
    bool isJail = (dynamic_cast<JailTile*>(t) != nullptr);
    if (isJail) {
        ci.setJailCell(true);
        int inmates = 0, visiting = 0;
        for (Player* p : game.getPlayers()) {
            if (p->getPosition() == t->getIndex()) {
                if (p->isJailed()) inmates++;
                else               visiting++;
            }
        }
        ci.setJailInmates(inmates);
        ci.setJailVisiting(visiting);
    }

    // player tokens (shown on non-jail tiles)
    if (!isJail) {
        const auto& allPlayers = game.getPlayers();
        for (int i = 0; i < (int)allPlayers.size(); ++i) {
            if (allPlayers[i]->getPosition() == t->getIndex()) {
                ci.appendPlayer("(" + std::to_string(i + 1) + ")");
            }
        }
    }

    return ci;
}

// Each cell is 10 chars wide to match the sample CLI board layout.
static const int CELL_W = 10;

static std::string fitLeft(const std::string& s, int w) {
    if ((int)s.size() >= w) return s.substr(0, w);
    return s + std::string(w - (int)s.size(), ' ');
}

static std::string padCenter(const std::string& s, int w) {
    if ((int)s.size() >= w) return s.substr(0, w);
    int left = (w - (int)s.size()) / 2;
    int right = w - (int)s.size() - left;
    return std::string(left, ' ') + s + std::string(right, ' ');
}

static std::string paint(TileColor color, const std::string& text) {
    if (color == TileColor::DEFAULT) return text;
    return colorPrefix(color) + text + RESET;
}

static std::string centreLine(const std::string& s, int width) {
    return padCenter(s, width);
}

static std::string padRight(const std::string& s, int width) {
    if ((int)s.size() >= width) return s.substr(0, width);
    return s + std::string(width - (int)s.size(), ' ');
}

static std::string centredBlockLine(const std::string& s, int blockWidth, int totalWidth) {
    return centreLine(padRight(s, blockWidth), totalWidth);
}

// Returns pair<row0, row1> as raw strings (no color escape inside content)
static std::pair<std::string,std::string> cellContent(const CellInfo& ci) {
    std::string tag  = "[" + ci.getColorTag() + "] " + ci.getCode();
    std::string row0 = fitLeft(tag, CELL_W);

    std::string info;
    if (ci.isJailCell()) {
        if (ci.getJailInmates()  > 0) info += "IN:" + std::to_string(ci.getJailInmates()) + " ";
        if (ci.getJailVisiting() > 0) info += "V:"  + std::to_string(ci.getJailVisiting());
    } else {
        info = ci.getOwnerTag();
        if (!ci.getBuilding().empty()) info += " " + ci.getBuilding();
        if (!ci.getPlayers().empty())  info += " " + ci.getPlayers();
    }
    std::string row1 = fitLeft(info, CELL_W);
    return {row0, row1};
}

static void printHLine(const std::vector<CellInfo>& cells) {
    std::cout << "+";
    for (const CellInfo& ci : cells) {
        std::cout << paint(ci.getColor(), std::string(CELL_W, '-')) << "+";
    }
    std::cout << "\n";
}

// Print one horizontal strip of cells (with color)
static void printCellRow(const std::vector<CellInfo>& cells, bool closeBottom = false) {
    printHLine(cells);
    if (cells.empty()) {
        std::cout << "\n\n";
        return;
    }

    auto [firstTop, firstBottom] = cellContent(cells.front());
    std::cout << paint(cells.front().getColor(), "|" + firstTop + "|");
    for (size_t i = 1; i < cells.size(); ++i) {
        auto [r0, r1] = cellContent(cells[i]);
        std::cout << paint(cells[i].getColor(), r0 + "|");
    }
    std::cout << "\n";

    std::cout << paint(cells.front().getColor(), "|" + firstBottom + "|");
    for (size_t i = 1; i < cells.size(); ++i) {
        auto [ignored, r1] = cellContent(cells[i]);
        std::cout << paint(cells[i].getColor(), r1 + "|");
    }
    std::cout << "\n";

    if (closeBottom) {
        printHLine(cells);
    }
}

// ── Middle rows (left cell + center panel + right cell) ───────────────────────

// center panel lines (fixed 9 * CELL_W + 8 = 98 chars wide)
static const int CENTER_W = 9 * (CELL_W + 1) - 1; // 9 inner cells

// Returns exactly 27 lines (separator + 2 content lines for each of 9 middle rows)
static std::vector<std::string> buildCenterPanel(const Game& game) {
    const int BLOCK_W = 40;
    const std::string titleBar = "====================================";
    const std::string divider = "------------------------------------";

    std::vector<std::string> lines = {
        std::string(CENTER_W, ' '),
        centredBlockLine(titleBar, BLOCK_W, CENTER_W),
        centredBlockLine("||           NIMONSPOLI           ||", BLOCK_W, CENTER_W),

        centredBlockLine(titleBar, BLOCK_W, CENTER_W),
        centreLine("TURN " + std::to_string(game.getCurrentTurn()) + " / "
                   + std::to_string(game.getMaxTurn()), CENTER_W),
        std::string(CENTER_W, ' '),

        centredBlockLine(divider, BLOCK_W, CENTER_W),
        centredBlockLine("LEGENDA KEPEMILIKAN & STATUS", BLOCK_W, CENTER_W),
        centredBlockLine("P1-P4 : Properti milik Pemain 1-4", BLOCK_W, CENTER_W),

        centredBlockLine("^     : Rumah Level 1", BLOCK_W, CENTER_W),
        centredBlockLine("^^    : Rumah Level 2", BLOCK_W, CENTER_W),
        centredBlockLine("^^^   : Rumah Level 3", BLOCK_W, CENTER_W),

        centredBlockLine("*     : Hotel (Maksimal)", BLOCK_W, CENTER_W),
        centredBlockLine("(1)-(4): Bidak (IN=Tahanan, V=Mampir)", BLOCK_W, CENTER_W),
        centredBlockLine(divider, BLOCK_W, CENTER_W),

        centredBlockLine("KODE WARNA:", BLOCK_W, CENTER_W),
        centredBlockLine("[K]=Coklat     [MR]=Merah", BLOCK_W, CENTER_W),
        centredBlockLine("[BM]=Biru Muda [KN]=Kuning", BLOCK_W, CENTER_W),

        centredBlockLine("[PK]=Pink      [HJ]=Hijau", BLOCK_W, CENTER_W),
        centredBlockLine("[OR]=Orange    [BT]=Biru Tua", BLOCK_W, CENTER_W),
        centredBlockLine("[DF]=Aksi      [AB]=Utilitas", BLOCK_W, CENTER_W),

        std::string(CENTER_W, ' '),
        std::string(CENTER_W, ' '),
        std::string(CENTER_W, ' '),

        std::string(CENTER_W, ' '),
        std::string(CENTER_W, ' '),
        std::string(CENTER_W, ' ')
    };
    return lines;
}

// ── Main renderBoard ──────────────────────────────────────────────────────────

void CLIGUI::renderBoard(const Game& game) {
    Board* board = game.getBoard();
    if (board == nullptr) { std::cout << "[INFO] Papan belum tersedia.\n"; return; }

    const auto& tiles = board->getAllTiles();
    int N = (int)tiles.size(); // expected 40

    // Build cell infos indexed by tile index (1-based → vector[idx-1])
    std::vector<CellInfo> ci(N);
    for (int i = 0; i < N; ++i) ci[i] = makeCellInfo(tiles[i], game);

    // Layout (1-based tile indices, Monopoly convention):
    // Bottom row L→R : 11,10,9,...,1         (tiles[10]..tiles[0])
    // Left col  B→T  : 12,13,...,20           (tiles[11]..tiles[19])
    // Top row   L→R  : 21,22,...,31           (tiles[20]..tiles[30])
    // Right col T→B  : 32,33,...,40           (tiles[31]..tiles[39])
    // corners included in their respective rows

    // ── TOP ROW (tiles 21-31) ─────────────────────────────────────────────
    std::vector<CellInfo> topRow;
    for (int idx = 21; idx <= 31; ++idx) topRow.push_back(ci[idx - 1]);
    printCellRow(topRow, true);

    // ── MIDDLE ROWS (left cell + center + right cell) ─────────────────────
    // buildCenterPanel returns 27 lines: line[row*3]   = separator content,
    //                                    line[row*3+1] = display-line-0,
    //                                    line[row*3+2] = display-line-1
    auto centerLines = buildCenterPanel(game);
    while ((int)centerLines.size() < 27) centerLines.push_back(std::string(CENTER_W, ' '));

    for (int row = 0; row < 9; ++row) {
        int leftIdx  = 20 - row; // 20,19,...,12
        int rightIdx = 32 + row; // 32,33,...,40

        CellInfo& leftCI  = ci[leftIdx  - 1];
        CellInfo& rightCI = ci[rightIdx - 1];

        auto [lR0, lR1] = cellContent(leftCI);
        auto [rR0, rR1] = cellContent(rightCI);

        const std::string& cSep   = centerLines[row * 3];
        const std::string& cLine0 = centerLines[row * 3 + 1];
        const std::string& cLine1 = centerLines[row * 3 + 2];

        // separator
        if (row != 0) {
            std::cout << "+" << paint(leftCI.getColor(), std::string(CELL_W, '-')) << "+"
                      << cSep
                      << "+" << paint(rightCI.getColor(), std::string(CELL_W, '-')) << "+\n";
        }

        // display line 0 (tag+code row)
        std::cout << paint(leftCI.getColor(), "|" + lR0 + "|")
                  << cLine0
                  << paint(rightCI.getColor(), "|" + rR0 + "|")
                  << "\n";

        // display line 1 (info row)
        std::cout << paint(leftCI.getColor(), "|" + lR1 + "|")
                  << cLine1
                  << paint(rightCI.getColor(), "|" + rR1 + "|")
                  << "\n";
    }

    // ── BOTTOM ROW (tiles 11 down to 1) ──────────────────────────────────
    std::vector<CellInfo> botRow;
    for (int idx = 11; idx >= 1; --idx) botRow.push_back(ci[idx - 1]);
    printCellRow(botRow, true);
}

// ── Everything else ───────────────────────────────────────────────────────────

CLIGUI::CLIGUI() : exitRequested(false), awaitingInput(false) {}

void CLIGUI::update() {}
void CLIGUI::display() {}

bool CLIGUI::shouldExit() const { return exitRequested; }

std::string CLIGUI::getCommand() {
    if (!pendingCommands.empty()) {
        std::string c = pendingCommands.front();
        pendingCommands.pop_front();
        return c;
    }
    std::cout << "> " << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) {
        exitRequested = true;
        return "CLOSE GAME";
    }
    if (line.empty()) return "NULL";
    return line;
}

void CLIGUI::loadMainMenu() {
    std::cout << "\n===== NIMONSPOLI =====\n"
              << "Ketik salah satu:\n"
              << "  NEW_GAME           - mulai game baru\n"
              << "  LOAD_GAME <path>   - muat save file\n"
              << "  EXIT               - keluar\n";
}

void CLIGUI::loadGameView() {
    std::cout << "\n----- Memulai Permainan -----\n";
}

void CLIGUI::loadFinishMenu() {
    std::cout << "\n----- Permainan Berakhir -----\n";
}

void CLIGUI::showMessage(const std::string& message) {
    std::cout << "[INFO] " << message << "\n";
}

void CLIGUI::showConfirm(const std::string& question) {
    std::cout << "[?] " << question << " (YA/TIDAK): " << std::flush;
}

void CLIGUI::showInputPrompt(const std::string& prompt) {
    std::cout << "[INPUT] " << prompt << "\n";
}

void CLIGUI::renderPlayer(const Player& player) {
    std::cout << "\n--- " << player.getUsername() << " ---\n"
              << "  Saldo   : " << player.getBalance() << "\n"
              << "  Posisi  : " << player.getPosition() << "\n"
              << "  Properti: " << player.getOwnedProperties().size() << "\n"
              << "  Kartu   : " << player.getCardCount() << "\n";
}

void CLIGUI::renderProperty(const Property& property) {
    std::cout << "[" << property.getCode() << "] " << property.getName()
              << " | harga: " << property.getPurchasePrice()
              << " | gadai: " << property.getMortgageValue();
    if (property.getOwner() != nullptr)
        std::cout << " | owner: " << property.getOwner()->getUsername();
    std::cout << "\n";
}

void CLIGUI::renderDice(int die1, int die2) {
    std::cout << "Hasil: " << die1 << " + " << die2 << " = " << (die1 + die2) << "\n";
}

void CLIGUI::renderLog(const std::vector<LogEntry>& entries) {
    std::cout << "\n--- LOG ---\n";
    for (const LogEntry& e : entries) {
        std::cout << "[turn " << e.getTurn() << "] "
                  << e.getUsername() << " " << e.getActionType()
                  << ": " << e.getDetail() << "\n";
    }
}

void CLIGUI::renderSkillHand(const std::vector<SkillCard*>& hand) {
    std::cout << "\n--- KARTU KEMAMPUAN ---\n";
    for (size_t i = 0; i < hand.size(); ++i)
        std::cout << i << ". " << hand[i]->getCardName() << "\n";
}

void CLIGUI::renderAuction(const Property& property, int currentBid, const Player* highBidder) {
    std::cout << "\n[LELANG] " << property.getName()
              << " | bid saat ini: " << currentBid;
    if (highBidder != nullptr)
        std::cout << " (oleh " << highBidder->getUsername() << ")";
    std::cout << "\n";
}

void CLIGUI::renderBankruptcy(const Player& player) {
    std::cout << "\n[BANGKRUT] " << player.getUsername()
              << " tidak dapat membayar. Likuidasi aset...\n";
}

void CLIGUI::renderWinner(const Player& winner) {
    std::cout << "\n===== PEMENANG =====\n"
              << winner.getUsername()
              << " dengan total kekayaan "
              << winner.calculateTotalWealth() << "\n";
}

void CLIGUI::renderMovement(const std::string& playerName, int steps, const std::string& landedTileName) {
    std::cout << "Memajukan Bidak " << playerName
              << " sebanyak " << steps << " petak...\n"
              << "Bidak mendarat di: " << landedTileName << ".\n";
}
