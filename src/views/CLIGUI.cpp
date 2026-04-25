#include "views/CLIGUI.hpp"
#include "models/BoardAndTiles/Board.hpp"
#include "models/BoardAndTiles/PropertyTile.hpp"
#include "models/BoardAndTiles/SpecialTile/JailTile.hpp"
#include "models/Property/StreetProperty.hpp"
#include "models/Property/RailroadProperty.hpp"
#include "models/Property/UtilityProperty.hpp"
#include "utils/Formatter.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>

namespace {
const std::string UI_RESET   = "\033[0m";
const std::string UI_BOLD    = "\033[1m";
const std::string UI_DIM     = "\033[2m";
const std::string UI_WHITE   = "\033[97m";
const std::string UI_CYAN    = "\033[38;5;81m";
const std::string UI_GREEN   = "\033[38;5;42m";
const std::string UI_YELLOW  = "\033[38;5;220m";
const std::string UI_RED     = "\033[38;5;203m";
const std::string UI_MAGENTA = "\033[38;5;177m";
const std::string UI_ORANGE  = "\033[38;5;208m";

enum class MessageTone {
    INFO,
    SUCCESS,
    WARNING,
    ERROR,
    ACTION
};

std::string propertyStatusLabel(const Property& property) {
    std::string label;
    switch (property.getStatus()) {
        case PropertyStatus::BANK: label = "BANK"; break;
        case PropertyStatus::OWNED: label = "OWNED"; break;
        case PropertyStatus::MORTGAGED: label = "MORTGAGED"; break;
    }
    if (property.getOwner() != nullptr) {
        label += " (" + property.getOwner()->getUsername() + ")";
    }
    return label;
}

std::string streetColorLabel(const std::string& colorGroup) {
    if (colorGroup == "COKLAT") return "COKLAT";
    if (colorGroup == "BIRU_MUDA") return "BIRU MUDA";
    if (colorGroup == "MERAH_MUDA") return "PINK";
    if (colorGroup == "ORANGE") return "ORANGE";
    if (colorGroup == "MERAH") return "MERAH";
    if (colorGroup == "KUNING") return "KUNING";
    if (colorGroup == "HIJAU") return "HIJAU";
    if (colorGroup == "BIRU_TUA") return "BIRU TUA";
    if (colorGroup == "ABU_ABU") return "ABU ABU";
    return colorGroup;
}

std::string propertyColorLabel(const Property& property) {
    if (property.isStreet()) {
        auto* street = static_cast<const StreetProperty*>(&property);
        return streetColorLabel(street->getColorGroup());
    }
    if (property.getType() == PropertyType::UTILITY) {
        return "ABU ABU";
    }
    return "DEFAULT";
}

std::string propertyGroupLabel(const Property& property) {
    if (property.isStreet()) {
        auto* street = static_cast<const StreetProperty*>(&property);
        return streetColorLabel(street->getColorGroup());
    }
    if (property.getType() == PropertyType::RAILROAD) {
        return "STASIUN";
    }
    if (property.getType() == PropertyType::UTILITY) {
        return "UTILITAS";
    }
    return "LAINNYA";
}

std::string buildingLabel(const Property& property) {
    if (!property.isStreet()) return "";
    auto* street = static_cast<const StreetProperty*>(&property);

    switch (street->getBuildingState()) {
        case BuildingState::HOUSE_1: return "1 rumah";
        case BuildingState::HOUSE_2: return "2 rumah";
        case BuildingState::HOUSE_3: return "3 rumah";
        case BuildingState::HOUSE_4: return "4 rumah";
        case BuildingState::HOTEL:   return "Hotel";
        default:                     return "";
    }
}

std::string shortStatusLabel(const Property& property) {
    switch (property.getStatus()) {
        case PropertyStatus::BANK:      return "BANK";
        case PropertyStatus::OWNED:     return "OWNED";
        case PropertyStatus::MORTGAGED: return "MORTGAGED [M]";
    }
    return "UNKNOWN";
}

std::string playerStatusLabel(PlayerStatus status) {
    switch (status) {
        case PlayerStatus::ACTIVE:   return "ACTIVE";
        case PlayerStatus::JAILED:   return "JAILED";
        case PlayerStatus::BANKRUPT: return "BANKRUPT";
    }
    return "ACTIVE";
}

std::string boxLine(const std::string& text, int innerWidth) {
    std::string body = text;
    if ((int)body.size() > innerWidth) body = body.substr(0, innerWidth);
    if ((int)body.size() < innerWidth) body += std::string(innerWidth - (int)body.size(), ' ');
    return "|" + body + "|";
}

std::string kvText(const std::string& key, const std::string& value) {
    const int keyWidth = 18;
    std::string left = key;
    if ((int)left.size() < keyWidth) {
        left += std::string(keyWidth - (int)left.size(), ' ');
    }
    return " " + left + " : " + value;
}

std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

bool containsAny(const std::string& text, const std::vector<std::string>& needles) {
    const std::string lowered = toLower(text);
    for (const std::string& needle : needles) {
        if (lowered.find(needle) != std::string::npos) return true;
    }
    return false;
}

std::string colorize(const std::string& text, const std::string& color, bool bold = false, bool dim = false) {
    std::string styled;
    if (bold) styled += UI_BOLD;
    if (dim) styled += UI_DIM;
    styled += color + text + UI_RESET;
    return styled;
}

std::string toneColor(MessageTone tone) {
    switch (tone) {
        case MessageTone::SUCCESS: return UI_GREEN;
        case MessageTone::WARNING: return UI_YELLOW;
        case MessageTone::ERROR:   return UI_RED;
        case MessageTone::ACTION:  return UI_ORANGE;
        case MessageTone::INFO:    return UI_CYAN;
    }
    return UI_CYAN;
}

std::string sectionHeaderColor(const std::string& line) {
    if (containsAny(line, {"giliran sekarang", "fase"})) return UI_WHITE;
    if (containsAny(line, {"aksi", "mode khusus"})) return UI_ORANGE;
    if (containsAny(line, {"properti", "uang"})) return UI_GREEN;
    if (containsAny(line, {"catatan"})) return UI_YELLOW;
    if (containsAny(line, {"informasi"})) return UI_CYAN;
    return UI_WHITE;
}

std::string helpTagColor(const std::string& tag) {
    if (tag == "TURN") return UI_WHITE;
    if (tag == "INFO") return UI_CYAN;
    if (tag == "ACTION") return UI_ORANGE;
    if (tag == "PROPERTY") return UI_GREEN;
    if (tag == "SPECIAL") return UI_MAGENTA;
    if (tag == "NOTE") return UI_YELLOW;
    return UI_CYAN;
}

bool renderTaggedHelpLine(const std::string& line) {
    const std::string marker = "@HELP:";
    if (line.rfind(marker, 0) != 0) return false;

    const size_t tagStart = marker.size();
    const size_t tagEnd = line.find(':', tagStart);
    if (tagEnd == std::string::npos) return false;

    const std::string tag = line.substr(tagStart, tagEnd - tagStart);
    const std::string text = line.substr(tagEnd + 1);
    const std::string color = helpTagColor(tag);

    if (tag == "NOTE") {
        std::cout << colorize("!", color, true) << "  "
                  << colorize(text, color) << "\n";
    } else {
        std::cout << colorize("|", color, true) << "  "
                  << colorize(text, color) << "\n";
    }
    return true;
}

std::string tonePrefix(MessageTone tone) {
    switch (tone) {
        case MessageTone::SUCCESS: return "OK";
        case MessageTone::WARNING: return "!";
        case MessageTone::ERROR:   return "X";
        case MessageTone::ACTION:  return ">";
        case MessageTone::INFO:    return "-";
    }
    return "-";
}

MessageTone classifyMessageTone(const std::string& message) {
    if (containsAny(message, {"gagal", "tidak ditemukan", "format save tidak dikenali",
                              "tidak valid", "tidak dapat", "ditolak", "belum tersedia"})) {
        return MessageTone::ERROR;
    }
    if (containsAny(message, {"peringatan", "dibatalkan", "sudah ada", "harus", "hanya dapat"})) {
        return MessageTone::WARNING;
    }
    if (containsAny(message, {"lelang", "gadai", "digadaikan", "tebus", "ditebus",
                              "pajak", "pph", "pbm", "bayar", "membayar", "dibayar",
                              "aksi lempar dadu selesai"})) {
        return MessageTone::ACTION;
    }
    if (containsAny(message, {"berhasil", "disimpan", "dimuat", "menjadi milikmu",
                              "kini menjadi milikmu", "masuk ke tanganmu", "mendapat"})) {
        return MessageTone::SUCCESS;
    }
    return MessageTone::INFO;
}

std::string statusColor(PlayerStatus status) {
    switch (status) {
        case PlayerStatus::ACTIVE:   return UI_GREEN;
        case PlayerStatus::JAILED:   return UI_YELLOW;
        case PlayerStatus::BANKRUPT: return UI_RED;
    }
    return UI_WHITE;
}

std::string bannerLine(char fill = '=') {
    return std::string(60, fill);
}

std::string centerText(const std::string& text, int width) {
    if ((int)text.size() >= width) return text.substr(0, width);
    int left = (width - (int)text.size()) / 2;
    int right = width - (int)text.size() - left;
    return std::string(left, ' ') + text + std::string(right, ' ');
}

void printBanner(const std::string& title, const std::string& color, char fill = '=') {
    const std::string line = bannerLine(fill);
    std::cout << "\n"
              << colorize(line, color, true) << "\n"
              << colorize(centerText(title, static_cast<int>(line.size())), color, true) << "\n"
              << colorize(line, color, true) << "\n";
}

}

// ── ANSI color constants ──────────────────────────────────────────────────────
const std::string CLIGUI::ANSI_RESET = "\033[0m";
const std::string CLIGUI::FG_BROWN   = "\033[38;5;130m";
const std::string CLIGUI::FG_LBLUE   = "\033[38;5;117m";
const std::string CLIGUI::FG_PINK    = "\033[38;5;201m";
const std::string CLIGUI::FG_ORANGE  = "\033[38;5;208m";
const std::string CLIGUI::FG_RED     = "\033[38;5;196m";
const std::string CLIGUI::FG_YELLOW  = "\033[38;5;220m";
const std::string CLIGUI::FG_GREEN   = "\033[38;5;76m";
const std::string CLIGUI::FG_DBLUE   = "\033[38;5;27m";
const std::string CLIGUI::FG_GRAY    = "\033[38;5;250m";
const std::string CLIGUI::FG_DEFAULT = "\033[39m";

const int CLIGUI::CELL_W   = 12;
const int CLIGUI::CENTER_W = 9 * (CLIGUI::CELL_W + 1) - 1;

std::string CLIGUI::colorPrefix(TileColor c) {
    switch (c) {
        case TileColor::COKLAT:     return CLIGUI::FG_BROWN;
        case TileColor::BIRU_MUDA:  return CLIGUI::FG_LBLUE;
        case TileColor::MERAH_MUDA: return CLIGUI::FG_PINK;
        case TileColor::ORANYE:     return CLIGUI::FG_ORANGE;
        case TileColor::MERAH:      return CLIGUI::FG_RED;
        case TileColor::KUNING:     return CLIGUI::FG_YELLOW;
        case TileColor::HIJAU:      return CLIGUI::FG_GREEN;
        case TileColor::BIRU_TUA:   return CLIGUI::FG_DBLUE;
        case TileColor::ABU_ABU:    return CLIGUI::FG_GRAY;
        default:                    return CLIGUI::FG_DEFAULT;
    }
}

std::string CLIGUI::colorTag(TileColor c) {
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

std::string CLIGUI::buildingStr(BuildingState s) {
    switch (s) {
        case BuildingState::HOUSE_1: return "^";
        case BuildingState::HOUSE_2: return "^^";
        case BuildingState::HOUSE_3: return "^^^";
        case BuildingState::HOUSE_4: return "^^^^";
        case BuildingState::HOTEL:   return "*";
        default:                     return "";
    }
}

CellInfo CLIGUI::makeCellInfo(Tile* t, const Game& game) {
    CellInfo ci;
    ci.setCode(t->getCode());
    ci.setColor(t->getColor());
    ci.setColorTag(colorTag(t->getColor()));

    // ownership & building
    if (t->getCategory() == TileCategory::PROPERTY) {
        auto* pt = static_cast<PropertyTile*>(t);
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
            if (p->isStreet()) {
                auto* sp = static_cast<StreetProperty*>(p);
                ci.setBuilding(buildingStr(sp->getBuildingState()));
            }
        }
    }

    // jail special
    bool isJail = (t->getKind() == TileKind::JAIL);
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

std::string CLIGUI::fitLeft(const std::string& s, int w) {
    if ((int)s.size() >= w) return s.substr(0, w);
    return s + std::string(w - (int)s.size(), ' ');
}

std::string CLIGUI::padCenter(const std::string& s, int w) {
    if ((int)s.size() >= w) return s.substr(0, w);
    int left = (w - (int)s.size()) / 2;
    int right = w - (int)s.size() - left;
    return std::string(left, ' ') + s + std::string(right, ' ');
}

std::string CLIGUI::paint(TileColor color, const std::string& text) {
    if (color == TileColor::DEFAULT) return text;
    return colorPrefix(color) + text + CLIGUI::ANSI_RESET;
}

std::string CLIGUI::centreLine(const std::string& s, int width) {
    return padCenter(s, width);
}

std::string CLIGUI::padRight(const std::string& s, int width) {
    if ((int)s.size() >= width) return s.substr(0, width);
    return s + std::string(width - (int)s.size(), ' ');
}

std::string CLIGUI::centredBlockLine(const std::string& s, int blockWidth, int totalWidth) {
    return centreLine(padRight(s, blockWidth), totalWidth);
}

std::pair<std::string,std::string> CLIGUI::cellContent(const CellInfo& ci) {
    std::string tag  = "[" + ci.getColorTag() + "] " + ci.getCode();
    std::string row0 = fitLeft(tag, CELL_W);

    std::string info;
    if (ci.isJailCell()) {
        if (ci.getJailInmates()  > 0) info += "IN:" + std::to_string(ci.getJailInmates()) + " ";
        if (ci.getJailVisiting() > 0) info += "V:"  + std::to_string(ci.getJailVisiting());
    } else {
        // Prioritize visible player tokens so all four pawns remain readable.
        if (!ci.getPlayers().empty()) {
            info = ci.getPlayers();
            if (!ci.getOwnerTag().empty()) info += " " + ci.getOwnerTag();
            if (!ci.getBuilding().empty()) info += " " + ci.getBuilding();
        } else {
            info = ci.getOwnerTag();
            if (!ci.getBuilding().empty()) info += " " + ci.getBuilding();
        }
    }
    std::string row1 = fitLeft(info, CELL_W);
    return {row0, row1};
}

void CLIGUI::printHLine(const std::vector<CellInfo>& cells) {
    std::cout << "+";
    for (const CellInfo& ci : cells) {
        std::cout << paint(ci.getColor(), std::string(CELL_W, '-')) << "+";
    }
    std::cout << "\n";
}

void CLIGUI::printCellRow(const std::vector<CellInfo>& cells, bool closeBottom) {
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

std::vector<std::string> CLIGUI::buildCenterPanel(const Game& game) {
    const int BLOCK_W = 40;
    const std::string titleBar = "====================================";
    const std::string divider = "------------------------------------";
    std::string playerLine1;
    std::string playerLine2;
    const auto& players = game.getPlayers();
    for (int i = 0; i < static_cast<int>(players.size()); ++i) {
        if (players[i] == nullptr) continue;
        std::string token = "(" + std::to_string(i + 1) + ") " + players[i]->getUsername();
        if (i < 2) {
            if (!playerLine1.empty()) playerLine1 += "   ";
            playerLine1 += token;
        } else {
            if (!playerLine2.empty()) playerLine2 += "   ";
            playerLine2 += token;
        }
    }
    if (playerLine1.empty()) playerLine1 = "-";
    if (playerLine2.empty()) playerLine2 = " ";

    std::vector<std::string> lines = {
        std::string(CENTER_W, ' '),
        centredBlockLine(titleBar, BLOCK_W, CENTER_W),
        centredBlockLine("||           NIMONSPOLI           ||", BLOCK_W, CENTER_W),

        centredBlockLine(titleBar, BLOCK_W, CENTER_W),
        centreLine("TURN " + std::to_string(game.getCurrentTurn()) + " / "
                   + std::to_string(game.getMaxTurn()), CENTER_W),
        centreLine("URUTAN BIDAK: " + playerLine1, CENTER_W),

        centreLine(playerLine2, CENTER_W),
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
        centredBlockLine("[CK]=Coklat    [MR]=Merah", BLOCK_W, CENTER_W),
        centredBlockLine("[BM]=Biru Muda [KN]=Kuning", BLOCK_W, CENTER_W),

        centredBlockLine("[PK]=Pink      [HJ]=Hijau", BLOCK_W, CENTER_W),
        centredBlockLine("[OR]=Orange    [BT]=Biru Tua", BLOCK_W, CENTER_W),
        centredBlockLine("[DF]=Aksi      [AB]=Utilitas", BLOCK_W, CENTER_W),

        std::string(CENTER_W, ' '),
        std::string(CENTER_W, ' '),
        std::string(CENTER_W, ' ')
    };
    return lines;
}

// ── Main renderBoard ──────────────────────────────────────────────────────────

void CLIGUI::renderBoard(const Game& game) {
    Board* board = game.getBoard();
    if (board == nullptr) {
        std::cout << colorize("X  Papan belum tersedia.", UI_RED, true) << "\n";
        return;
    }

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

CLIGUI::CLIGUI() : exitRequested(false), awaitingInput(false), winnerBannerPrinted(false) {}

void CLIGUI::update() {}
void CLIGUI::display() {}

bool CLIGUI::shouldExit() const { return exitRequested; }

std::string CLIGUI::getCommand() {
    if (!pendingCommands.empty()) {
        std::string c = pendingCommands.front();
        pendingCommands.pop_front();
        return c;
    }
    std::cout << colorize("> ", UI_MAGENTA, true) << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) {
        exitRequested = true;
        return "CLOSE GAME";
    }
    if (line.empty()) return "NULL";
    return line;
}

void CLIGUI::loadMainMenu() {
    printBanner("NIMONSPOLI", UI_CYAN);
    std::cout << colorize("Menu Utama", UI_WHITE, true) << "\n"
              << "  NEW_GAME           mulai game baru\n"
              << "  LOAD_GAME <path>   muat save file\n"
              << "  MUAT <path>        alias load save file\n"
              << "  EXIT               keluar\n\n"
              << colorize("Warna pesan:", UI_WHITE, true) << " "
              << colorize("info", UI_CYAN) << " | "
              << colorize("berhasil", UI_GREEN) << " | "
              << colorize("aksi", UI_ORANGE) << " | "
              << colorize("peringatan", UI_YELLOW) << " | "
              << colorize("gagal", UI_RED) << " | "
              << colorize("input", UI_MAGENTA) << "\n";
}

void CLIGUI::loadGameView() {
    printBanner("PERMAINAN DIMULAI", UI_GREEN, '-');
}

void CLIGUI::loadFinishMenu() {
    winnerBannerPrinted = false;
    printBanner("PERMAINAN BERAKHIR", UI_RED, '-');
}

void CLIGUI::showMessage(const std::string& message) {
    const MessageTone tone = classifyMessageTone(message);
    const std::string prefix = tonePrefix(tone);
    const std::string color = toneColor(tone);

    std::stringstream ss(message);
    std::string line;
    while (std::getline(ss, line)) {
        if (renderTaggedHelpLine(line)) {
            continue;
        }
        if (line.rfind("===", 0) == 0) {
            std::cout << "\n" << colorize(line, sectionHeaderColor(line), true) << "\n";
            continue;
        }
        std::cout << colorize(prefix, color, true) << "  "
                  << colorize(line, color) << "\n";
    }
}

void CLIGUI::showConfirm(const std::string& question) {
    std::cout << colorize("?", UI_YELLOW, true) << "  "
              << colorize(question, UI_YELLOW, true) << " " << std::flush;
}

void CLIGUI::showInputPrompt(const std::string& prompt) {
    std::cout << colorize(prompt, UI_MAGENTA, true) << "\n";
}

void CLIGUI::renderPlayer(const Player& player) {
    std::cout << "\n"
              << colorize("Giliran " + player.getUsername(), UI_WHITE, true) << "\n"
              << colorize(std::string(24, '-'), UI_WHITE, false, true) << "\n"
              << "  Saldo    : " << colorize(Formatter::money(player.getBalance()), UI_GREEN, true) << "\n"
              << "  Posisi   : petak " << player.getPosition() << "\n"
              << "  Status   : " << colorize(playerStatusLabel(player.getStatus()), statusColor(player.getStatus()), true) << "\n"
              << "  Properti : " << player.getOwnedProperties().size() << "\n"
              << "  Kartu    : " << player.getCardCount() << "\n";
}

void CLIGUI::renderProperty(const Property& property) {
    std::string title = "[" + propertyColorLabel(property) + "] " +
                        property.getName() + " (" + property.getCode() + ")";

    std::vector<std::string> mainLines = {
        kvText("Harga Beli", Formatter::money(property.getPurchasePrice())),
        kvText("Nilai Gadai", Formatter::money(property.getMortgageValue()))
    };
    std::vector<std::string> detailLines;
    std::vector<std::string> buildLines;

    if (property.isStreet()) {
        auto* street = static_cast<const StreetProperty*>(&property);
        const auto& rents = street->getRentLevels();
        if (!rents.empty()) {
            static const std::vector<std::string> rentLabels = {
                "Sewa (unimproved)",
                "Sewa (1 rumah)",
                "Sewa (2 rumah)",
                "Sewa (3 rumah)",
                "Sewa (4 rumah)",
                "Sewa (hotel)"
            };
            for (size_t i = 0; i < rents.size() && i < rentLabels.size(); ++i) {
                detailLines.push_back(kvText(rentLabels[i], Formatter::money(rents[i])));
            }
        }
        buildLines.push_back(kvText("Harga Rumah", Formatter::money(street->getHouseBuildCost())));
        buildLines.push_back(kvText("Harga Hotel", Formatter::money(street->getHotelBuildCost())));
    } else if (property.isRailroad()) {
        auto* railroad = static_cast<const RailroadProperty*>(&property);
        for (const auto& [count, rent] : railroad->getRentTable()) {
            detailLines.push_back(kvText("Sewa (" + std::to_string(count) + " stasiun)",
                                         Formatter::money(rent)));
        }
    } else if (property.isUtility()) {
        auto* utility = static_cast<const UtilityProperty*>(&property);
        for (const auto& [count, mult] : utility->getMultiplierTable()) {
            detailLines.push_back(kvText("Sewa (" + std::to_string(count) + " utilitas)",
                                         std::to_string(mult) + "x dadu"));
        }
    }

    std::string statusLine = kvText("Status", propertyStatusLabel(property));

    int innerWidth = 30;
    innerWidth = std::max(innerWidth, (int)std::string("AKTA KEPEMILIKAN").size() + 2);
    innerWidth = std::max(innerWidth, (int)title.size() + 2);
    innerWidth = std::max(innerWidth, (int)statusLine.size() + 1);
    for (const std::string& line : mainLines) innerWidth = std::max(innerWidth, (int)line.size() + 1);
    for (const std::string& line : detailLines) innerWidth = std::max(innerWidth, (int)line.size() + 1);
    for (const std::string& line : buildLines) innerWidth = std::max(innerWidth, (int)line.size() + 1);

    const std::string top    = "+" + std::string(innerWidth, '=') + "+";
    const std::string middle = "+" + std::string(innerWidth, '-') + "+";
    const std::string titleColor = property.getType() == PropertyType::RAILROAD ? UI_YELLOW :
                                   property.getType() == PropertyType::UTILITY ? UI_WHITE :
                                   UI_CYAN;

    std::cout << colorize(top, titleColor, true) << "\n";
    std::cout << boxLine(padCenter("AKTA KEPEMILIKAN", innerWidth), innerWidth) << "\n";
    std::cout << colorize(boxLine(padCenter(title, innerWidth), innerWidth), titleColor, true) << "\n";
    std::cout << colorize(top, titleColor, true) << "\n";

    for (const std::string& line : mainLines) {
        std::cout << boxLine(line, innerWidth) << "\n";
    }

    if (!detailLines.empty()) {
        std::cout << colorize(middle, UI_WHITE, false, true) << "\n";
        for (const std::string& line : detailLines) {
            std::cout << boxLine(line, innerWidth) << "\n";
        }
    }

    if (!buildLines.empty()) {
        std::cout << colorize(middle, UI_WHITE, false, true) << "\n";
        for (const std::string& line : buildLines) {
            std::cout << boxLine(line, innerWidth) << "\n";
        }
    }

    std::cout << colorize(top, titleColor, true) << "\n";
    std::cout << boxLine(statusLine, innerWidth) << "\n";
    std::cout << colorize(top, titleColor, true) << "\n";
}

void CLIGUI::renderOwnedProperties(const Player& player) {
    const auto& owned = player.getOwnedProperties();

    std::vector<std::string> groupOrder;
    std::map<std::string, std::vector<Property*>> grouped;
    for (Property* property : owned) {
        if (property == nullptr) continue;
        std::string group = propertyGroupLabel(*property);
        if (grouped.find(group) == grouped.end()) {
            groupOrder.push_back(group);
        }
        grouped[group].push_back(property);
    }

    std::cout << "\n" << colorize("PROPERTI MILIK " + player.getUsername(), UI_WHITE, true) << "\n"
              << colorize(std::string(32, '='), UI_WHITE, false, true) << "\n\n";

    for (size_t g = 0; g < groupOrder.size(); ++g) {
        const std::string& group = groupOrder[g];
        std::cout << colorize("[" + group + "]", UI_CYAN, true) << "\n";

        for (Property* property : grouped[group]) {
            std::ostringstream line;
            std::string nameCode = "- " + property->getName() + " (" + property->getCode() + ")";
            line << "  " << std::left << std::setw(28) << nameCode;

            std::string building = buildingLabel(*property);
            line << "  " << std::left << std::setw(10) << building;

            line << " " << std::left << std::setw(8) << Formatter::money(property->getPurchasePrice());
            line << " " << shortStatusLabel(*property);

            std::cout << line.str() << "\n";
        }

        if (g + 1 < groupOrder.size()) {
            std::cout << "\n";
        }
    }

    std::cout << "\nTotal kekayaan properti: "
              << colorize(Formatter::money(player.calculatePropertyAssetValue() + player.calculateBuildingAssetValue()),
                          UI_GREEN, true)
              << "\n";
}

void CLIGUI::renderDice(int die1, int die2) {
    std::cout << colorize("Hasil dadu", UI_WHITE, true) << ": "
              << colorize(std::to_string(die1), UI_CYAN, true)
              << " + "
              << colorize(std::to_string(die2), UI_CYAN, true)
              << " = "
              << colorize(std::to_string(die1 + die2), UI_GREEN, true) << "\n";
}

void CLIGUI::renderLog(const std::vector<LogEntry>& entries, const std::string& title) {
    std::cout << "\n" << colorize(title, UI_WHITE, true) << "\n"
              << colorize(std::string(title.size(), '-'), UI_WHITE, false, true) << "\n";
    for (const LogEntry& e : entries) {
        std::cout << e.toString() << "\n";
    }
}

void CLIGUI::renderSkillHand(const std::vector<SkillCard*>& hand) {
    std::cout << "\n" << colorize("KARTU KEMAMPUAN", UI_MAGENTA, true) << "\n";
    for (size_t i = 0; i < hand.size(); ++i)
        std::cout << "  " << colorize(std::to_string(i + 1) + ".", UI_MAGENTA, true)
                  << " " << hand[i]->getCardName() << "\n";
}

void CLIGUI::renderAuction(const Property& property, int currentBid, const Player* highBidder) {
    std::cout << "\n" << colorize("LELANG", UI_ORANGE, true) << "  "
              << property.getName()
              << " | bid saat ini: " << colorize(Formatter::money(currentBid), UI_GREEN, true);
    if (highBidder != nullptr)
        std::cout << " (oleh " << colorize(highBidder->getUsername(), UI_WHITE, true) << ")";
    std::cout << "\n";
}

void CLIGUI::renderBankruptcy(const Player& player) {
    std::cout << "\n" << colorize("BANGKRUT", UI_RED, true) << "  "
              << colorize(player.getUsername(), UI_RED, true)
              << " tidak dapat membayar. Likuidasi aset...\n";
}

void CLIGUI::renderWinner(const Player& winner) {
    if (!winnerBannerPrinted) {
        printBanner("PEMENANG", UI_GREEN);
        winnerBannerPrinted = true;
    }
    std::cout << colorize(winner.getUsername(), UI_GREEN, true)
              << " dengan total kekayaan "
              << colorize(Formatter::money(winner.calculateTotalWealth()), UI_GREEN, true) << "\n";
}

void CLIGUI::renderMovement(const std::string& playerName, int steps, const std::string& landedTileName) {
    std::cout << colorize("Pergerakan", UI_CYAN, true) << "\n"
              << "  Bidak " << colorize(playerName, UI_WHITE, true)
              << " maju " << colorize(std::to_string(steps), UI_CYAN, true)
              << " petak.\n"
              << "  Mendarat di: " << colorize(landedTileName, UI_CYAN, true) << ".\n";
}
