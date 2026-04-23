#include "core/SaveLoadManager.hpp"
#include "models/BoardAndTiles/Board.hpp"
#include "models/BoardAndTiles/PropertyTile.hpp"
#include "models/BoardAndTiles/Tile.hpp"
#include "models/CardAndDeck/CardDeck.hpp"
#include "models/CardAndDeck/ChanceCard.hpp"
#include "models/CardAndDeck/CommunityChestCard.hpp"
#include "models/CardAndDeck/DemolitionCard.hpp"
#include "models/CardAndDeck/DiscountCard.hpp"
#include "models/CardAndDeck/LassoCard.hpp"
#include "models/CardAndDeck/MoveCard.hpp"
#include "models/CardAndDeck/ShieldCard.hpp"
#include "models/CardAndDeck/SkillCard.hpp"
#include "models/CardAndDeck/TeleportCard.hpp"
#include "models/Player/Player.hpp"
#include "models/Property/Property.hpp"
#include "models/Property/StreetProperty.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <type_traits>
#include <vector>

namespace {
constexpr int kSaveVersion = 2;
constexpr int kLogVersion = 2;

const std::vector<std::string> kConfigFiles = {
    "aksi.txt",
    "misc.txt",
    "property.txt",
    "railroad.txt",
    "special.txt",
    "tax.txt",
    "utility.txt"
};

std::vector<std::string> splitRaw(const std::string& s, char sep) {
    std::vector<std::string> out;
    std::stringstream ss(s);
    std::string tok;
    while (std::getline(ss, tok, sep)) out.push_back(tok);
    return out;
}

std::vector<std::string> splitEscaped(const std::string& s, char sep) {
    std::vector<std::string> out;
    std::string current;
    bool escaped = false;

    for (char ch : s) {
        if (escaped) {
            current.push_back(ch);
            escaped = false;
            continue;
        }

        if (ch == '\\') {
            escaped = true;
            continue;
        }

        if (ch == sep) {
            out.push_back(current);
            current.clear();
            continue;
        }

        current.push_back(ch);
    }

    if (escaped) current.push_back('\\');
    out.push_back(current);
    return out;
}

std::string escapeField(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char ch : s) {
        if (ch == '\\' || ch == '|' || ch == ',') out.push_back('\\');
        out.push_back(ch);
    }
    return out;
}

bool isIntegerToken(const std::string& s) {
    if (s.empty()) return false;
    std::size_t start = (s[0] == '-') ? 1 : 0;
    if (start == s.size()) return false;
    for (std::size_t i = start; i < s.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
    }
    return true;
}

std::string chanceTypeToToken(ChanceType type) {
    switch (type) {
        case ChanceType::GO_TO_NEAREST_STATION: return "GO_TO_NEAREST_STATION";
        case ChanceType::MOVE_BACK_3:           return "MOVE_BACK_3";
        case ChanceType::GO_TO_JAIL:            return "GO_TO_JAIL";
    }
    return "GO_TO_JAIL";
}

ChanceType chanceTypeFromToken(const std::string& token) {
    if (token == "GO_TO_NEAREST_STATION") return ChanceType::GO_TO_NEAREST_STATION;
    if (token == "MOVE_BACK_3")           return ChanceType::MOVE_BACK_3;
    return ChanceType::GO_TO_JAIL;
}

std::string communityTypeToToken(CommunityType type) {
    switch (type) {
        case CommunityType::BIRTHDAY:     return "BIRTHDAY";
        case CommunityType::DOCTOR_FEE:   return "DOCTOR_FEE";
        case CommunityType::CAMPAIGN_FEE: return "CAMPAIGN_FEE";
    }
    return "CAMPAIGN_FEE";
}

CommunityType communityTypeFromToken(const std::string& token) {
    if (token == "BIRTHDAY")     return CommunityType::BIRTHDAY;
    if (token == "DOCTOR_FEE")   return CommunityType::DOCTOR_FEE;
    return CommunityType::CAMPAIGN_FEE;
}

int skillCardValue(const SkillCard* card) {
    if (auto* move = dynamic_cast<const MoveCard*>(card)) {
        return move->getSteps();
    }
    if (auto* discount = dynamic_cast<const DiscountCard*>(card)) {
        return discount->getDiscount();
    }
    return 0;
}

int skillCardDuration(const SkillCard* card) {
    if (auto* discount = dynamic_cast<const DiscountCard*>(card)) {
        return discount->getDuration();
    }
    if (auto* shield = dynamic_cast<const ShieldCard*>(card)) {
        return shield->getDuration();
    }
    return 0;
}

std::string encodeCardToken(const Card* card) {
    if (auto* chance = dynamic_cast<const ChanceCard*>(card)) {
        return "ChanceCard:" + chanceTypeToToken(chance->getType());
    }
    if (auto* community = dynamic_cast<const CommunityChestCard*>(card)) {
        return "CommunityChestCard:" + communityTypeToToken(community->getType());
    }
    if (auto* skill = dynamic_cast<const SkillCard*>(card)) {
        return skill->getCardName() + ":" +
               std::to_string(skillCardValue(skill)) + ":" +
               std::to_string(skillCardDuration(skill));
    }
    return card != nullptr ? card->getCardName() : "";
}

template <typename T>
std::string joinCardTokens(const std::vector<T*>& cards) {
    std::ostringstream oss;
    for (std::size_t i = 0; i < cards.size(); ++i) {
        if (cards[i] != nullptr) {
            oss << escapeField(encodeCardToken(cards[i]));
        }
        if (i + 1 < cards.size()) {
            oss << ",";
        }
    }
    return oss.str();
}

SkillCard* decodeSkillCardToken(const std::string& token) {
    const auto parts = splitRaw(token, ':');
    if (parts.empty()) return nullptr;

    const std::string& name = parts[0];
    const int value = (parts.size() >= 2 && isIntegerToken(parts[1])) ? std::stoi(parts[1]) : 0;

    if (name == "MoveCard")       return new MoveCard(value == 0 ? 3 : value);
    if (name == "DiscountCard")   return new DiscountCard(value == 0 ? 50 : value);
    if (name == "ShieldCard")     return new ShieldCard();
    if (name == "TeleportCard")   return new TeleportCard();
    if (name == "LassoCard")      return new LassoCard();
    if (name == "DemolitionCard") return new DemolitionCard();
    return nullptr;
}

ChanceCard* decodeChanceCardToken(const std::string& token) {
    const auto parts = splitRaw(token, ':');
    if (parts.empty()) return nullptr;

    if (parts[0] == "ChanceCard" && parts.size() >= 2) {
        return new ChanceCard(chanceTypeFromToken(parts[1]));
    }
    if (parts[0] == "ChanceCard") {
        return new ChanceCard(ChanceType::GO_TO_JAIL);
    }
    return nullptr;
}

CommunityChestCard* decodeCommunityCardToken(const std::string& token) {
    const auto parts = splitRaw(token, ':');
    if (parts.empty()) return nullptr;

    if (parts[0] == "CommunityChestCard" && parts.size() >= 2) {
        return new CommunityChestCard(communityTypeFromToken(parts[1]));
    }
    if (parts[0] == "CommunityChestCard") {
        return new CommunityChestCard(CommunityType::CAMPAIGN_FEE);
    }
    return nullptr;
}

std::string propertyTypeToString(PropertyType type) {
    switch (type) {
        case PropertyType::STREET:   return "street";
        case PropertyType::RAILROAD: return "railroad";
        case PropertyType::UTILITY:  return "utility";
    }
    return "street";
}

std::string positionToToken(const Player* player, const Board* board) {
    if (player == nullptr) return "1";
    if (board != nullptr) {
        try {
            Tile* tile = board->getTile(player->getPosition());
            if (tile != nullptr && !tile->getCode().empty()) {
                return tile->getCode();
            }
        } catch (...) {
        }
    }
    return std::to_string(player->getPosition());
}

int positionFromToken(const std::string& token, const Board* board) {
    if (isIntegerToken(token)) return std::stoi(token);
    if (board == nullptr) return 1;

    try {
        Tile* tile = board->getTile(token);
        return (tile != nullptr) ? tile->getIndex() : 1;
    } catch (...) {
        return 1;
    }
}

std::string buildingToken(int stateIdx) {
    if (stateIdx <= 0) return "0";
    if (stateIdx >= 5) return "H";
    return std::to_string(stateIdx);
}

int parseBuildingToken(const std::string& token) {
    if (token == "H" || token == "HOTEL") return 5;
    if (!isIntegerToken(token)) return 0;
    const int value = std::stoi(token);
    return std::clamp(value, 0, 4);
}

int findPlayerIndex(const std::vector<Player*>& players, const std::string& username) {
    for (std::size_t i = 0; i < players.size(); ++i) {
        if (players[i] != nullptr && players[i]->getUsername() == username) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
}

SaveLoadManager::SaveLoadManager(Game* game, TransactionLogger* logger, TurnManager* turnManager, IGUI* gui,
                                 std::string configSourceDir)
    : game(game),
      logger(logger),
      turnManager(turnManager),
      gui(gui),
      configSourceDir(std::move(configSourceDir)) {}

bool SaveLoadManager::saveTargetExists(const std::string& filepath) const {
    namespace fs = std::filesystem;
    const fs::path saveDir(resolveSaveDirectory(filepath));
    return fs::exists(saveDir);
}

std::string SaveLoadManager::resolveSaveDirectory(const std::string& savePath) {
    namespace fs = std::filesystem;

    fs::path raw(savePath);
    if (raw.has_parent_path()) {
        return raw.string();
    }

    return (fs::path("data") / raw).string();
}

std::string SaveLoadManager::buildStateFilepath(const std::string& savePath) {
    namespace fs = std::filesystem;
    return (fs::path(resolveSaveDirectory(savePath)) / "game_state.txt").string();
}

std::string SaveLoadManager::buildLogFilepath(const std::string& savePath) {
    namespace fs = std::filesystem;
    return (fs::path(resolveSaveDirectory(savePath)) / "log.txt").string();
}

std::string SaveLoadManager::statusToString(PlayerStatus s) {
    switch (s) {
        case PlayerStatus::ACTIVE:   return "ACTIVE";
        case PlayerStatus::JAILED:   return "JAILED";
        case PlayerStatus::BANKRUPT: return "BANKRUPT";
    }
    return "ACTIVE";
}

PlayerStatus SaveLoadManager::parsePlayerStatus(const std::string& s) {
    if (s == "JAILED")   return PlayerStatus::JAILED;
    if (s == "BANKRUPT") return PlayerStatus::BANKRUPT;
    return PlayerStatus::ACTIVE;
}

std::string SaveLoadManager::propStatusToString(PropertyStatus s) {
    switch (s) {
        case PropertyStatus::BANK:      return "BANK";
        case PropertyStatus::OWNED:     return "OWNED";
        case PropertyStatus::MORTGAGED: return "MORTGAGED";
    }
    return "BANK";
}

PropertyStatus SaveLoadManager::parsePropStatus(const std::string& s) {
    if (s == "OWNED")     return PropertyStatus::OWNED;
    if (s == "MORTGAGED") return PropertyStatus::MORTGAGED;
    return PropertyStatus::BANK;
}

std::string SaveLoadManager::buildingToString(int stateIdx) {
    switch (stateIdx) {
        case 0: return "NONE";
        case 1: return "HOUSE_1";
        case 2: return "HOUSE_2";
        case 3: return "HOUSE_3";
        case 4: return "HOUSE_4";
        case 5: return "HOTEL";
    }
    return "NONE";
}

int SaveLoadManager::parseBuildingState(const std::string& s) {
    if (s == "HOUSE_1") return 1;
    if (s == "HOUSE_2") return 2;
    if (s == "HOUSE_3") return 3;
    if (s == "HOUSE_4") return 4;
    if (s == "HOTEL")   return 5;
    return 0;
}

Player* SaveLoadManager::findPlayerByUsername(const std::string& name) const {
    for (Player* p : game->getPlayers()) {
        if (p->getUsername() == name) return p;
    }
    return nullptr;
}

bool SaveLoadManager::saveLogFile(const std::string& saveFilepath) const {
    if (logger == nullptr) return true;

    std::ofstream logOfs(buildLogFilepath(saveFilepath));
    if (!logOfs.is_open()) return false;

    const auto& entries = logger->getFullLog();
    logOfs << "NIMONSPOLI_LOG " << kLogVersion << "\n";
    logOfs << "COUNT " << entries.size() << "\n";
    for (const LogEntry& entry : entries) {
        logOfs << "ENTRY "
               << entry.getTurn() << "|"
               << escapeField(entry.getUsername()) << "|"
               << escapeField(entry.getActionType()) << "|"
               << escapeField(entry.getDetail()) << "\n";
    }
    return true;
}

bool SaveLoadManager::saveConfigSnapshot(const std::string& savePath) const {
    namespace fs = std::filesystem;

    const fs::path saveDir(resolveSaveDirectory(savePath));
    const fs::path baseDir(configSourceDir.empty() ? "data/config/default" : configSourceDir);

    for (const std::string& file : kConfigFiles) {
        const fs::path source = baseDir / file;
        const fs::path target = saveDir / file;
        std::error_code ec;
        fs::copy_file(source, target, fs::copy_options::overwrite_existing, ec);
        if (ec) return false;
    }

    return true;
}

bool SaveLoadManager::saveDeckState(const std::string& savePath) const {
    if (game == nullptr) return false;

    std::ofstream ofs(buildStateFilepath(savePath), std::ios::app);
    if (!ofs.is_open()) return false;

    if (auto* deck = game->getChanceDeck(); deck != nullptr) {
        ofs << "DECK CHANCE|DRAW|" << joinCardTokens(deck->getDrawPile()) << "\n";
        ofs << "DECK CHANCE|DISCARD|" << joinCardTokens(deck->getDiscardPile()) << "\n";
    }
    if (auto* deck = game->getCommunityDeck(); deck != nullptr) {
        ofs << "DECK COMMUNITY|DRAW|" << joinCardTokens(deck->getDrawPile()) << "\n";
        ofs << "DECK COMMUNITY|DISCARD|" << joinCardTokens(deck->getDiscardPile()) << "\n";
    }
    if (auto* deck = game->getSkillDeck(); deck != nullptr) {
        ofs << "DECK SKILL|DRAW|" << joinCardTokens(deck->getDrawPile()) << "\n";
        ofs << "DECK SKILL|DISCARD|" << joinCardTokens(deck->getDiscardPile()) << "\n";
    }

    return true;
}

bool SaveLoadManager::loadLogFile(const std::string& saveFilepath, std::vector<LogEntry>& outEntries) const {
    auto parseModernLog = [](std::istream& logIfs, std::vector<LogEntry>& parsedEntries) -> bool {
        std::string line;
        if (!std::getline(logIfs, line)) {
            parsedEntries.clear();
            return true;
        }

        if (line.rfind("NIMONSPOLI_LOG", 0) != 0) {
            return false;
        }

        int expectedCount = 0;
        if (!std::getline(logIfs, line)) return false;
        if (line.rfind("COUNT ", 0) == 0 && isIntegerToken(line.substr(6))) {
            expectedCount = std::stoi(line.substr(6));
        }

        std::vector<LogEntry> parsed;
        parsed.reserve(std::max(expectedCount, 0));

        while (std::getline(logIfs, line)) {
            if (line.empty()) continue;
            if (line.rfind("ENTRY ", 0) != 0) continue;

            const auto fields = splitEscaped(line.substr(6), '|');
            if (fields.size() < 4 || !isIntegerToken(fields[0])) return false;

            parsed.emplace_back(std::stoi(fields[0]), fields[1], fields[2], fields[3]);
        }

        parsedEntries = std::move(parsed);
        return true;
    };

    auto parseLegacyLog = [](std::istream& logIfs, std::vector<LogEntry>& parsedEntries) -> bool {
        std::string line;
        if (!std::getline(logIfs, line)) {
            parsedEntries.clear();
            return true;
        }

        int expectedCount = 0;
        try {
            expectedCount = std::stoi(line);
        } catch (...) {
            return false;
        }

        std::vector<LogEntry> parsed;
        parsed.reserve(std::max(expectedCount, 0));

        while (std::getline(logIfs, line)) {
            if (line.empty()) continue;

            std::istringstream iss(line);
            int turn = 0;
            std::string username;
            std::string actionType;
            if (!(iss >> turn >> username >> actionType)) {
                return false;
            }

            std::string detail;
            std::getline(iss, detail);
            if (!detail.empty() && detail.front() == ' ') {
                detail.erase(0, 1);
            }
            parsed.emplace_back(turn, username, actionType, detail);
        }

        parsedEntries = std::move(parsed);
        return true;
    };

    auto tryParse = [&](const std::string& path) -> bool {
        std::ifstream ifs(path);
        if (!ifs.is_open()) return false;

        std::vector<LogEntry> entries;
        if (parseModernLog(ifs, entries)) {
            outEntries = std::move(entries);
            return true;
        }

        ifs.clear();
        ifs.seekg(0);
        if (parseLegacyLog(ifs, entries)) {
            outEntries = std::move(entries);
            return true;
        }

        return false;
    };

    if (tryParse(buildLogFilepath(saveFilepath))) return true;
    if (tryParse(saveFilepath + ".log")) return true;
    return false;
}

bool SaveLoadManager::save(const std::string& filepath) {
    if (game == nullptr) return false;

    namespace fs = std::filesystem;
    std::error_code ec;
    fs::create_directories(resolveSaveDirectory(filepath), ec);
    if (ec) {
        if (gui) gui->showMessage("Gagal menyiapkan folder data simpan: " + resolveSaveDirectory(filepath));
        return false;
    }

    std::ofstream ofs(buildStateFilepath(filepath));
    if (!ofs.is_open()) {
        if (gui) gui->showMessage("Gagal membuka data simpan: " + buildStateFilepath(filepath));
        return false;
    }

    ofs << "NIMONSPOLI_SAVE " << kSaveVersion << "\n";
    ofs << "GAME " << game->getCurrentTurn()
        << "|" << game->getMaxTurn()
        << "|" << game->getPlayers().size()
        << "|" << escapeField(game->getCurrentPlayer() ? game->getCurrentPlayer()->getUsername() : "")
        << "|" << game->getLastDiceTotal()
        << "|" << (game->isGameOver() ? 1 : 0)
        << "\n";
    ofs << "TURN " << static_cast<int>(turnManager ? turnManager->getPhase() : TurnPhase::START)
        << "|" << (turnManager && turnManager->hasActedThisTurn() ? 1 : 0)
        << "|" << (turnManager && turnManager->isShieldActive() ? 1 : 0)
        << "\n";

    ofs << "TURN_ORDER ";
    const auto& order = game->getTurnOrder();
    for (std::size_t i = 0; i < order.size(); ++i) {
        Player* player = game->getPlayer(order[i]);
        if (player != nullptr) {
            ofs << escapeField(player->getUsername());
        }
        if (i + 1 < order.size()) ofs << ",";
    }
    ofs << "\n";

    Board* board = game->getBoard();
    for (Player* p : game->getPlayers()) {
        ofs << "PLAYER " << escapeField(p->getUsername())
            << "|" << p->getBalance()
            << "|" << escapeField(positionToToken(p, board))
            << "|" << statusToString(p->getStatus())
            << "|" << p->getConsecutiveDoubles()
            << "|" << p->getJailAttempts()
            << "|" << (p->hasRolled() ? 1 : 0)
            << "|" << (p->hasUsedSkill() ? 1 : 0)
            << "|" << (p->hasPendingFestival() ? 1 : 0)
            << "\n";

        for (SkillCard* card : p->getHandCards()) {
            if (card == nullptr) continue;
            ofs << "CARD " << escapeField(p->getUsername())
                << "|" << escapeField(card->getCardName())
                << "|" << skillCardValue(card)
                << "|" << skillCardDuration(card)
                << "\n";
        }
    }

    if (board != nullptr) {
        for (Tile* t : board->getAllTiles()) {
            auto* pt = dynamic_cast<PropertyTile*>(t);
            if (pt == nullptr) continue;
            Property* prop = pt->getProperty();
            if (prop == nullptr) continue;

            std::string ownerName = prop->getOwner() ? prop->getOwner()->getUsername() : "BANK";
            int buildingIdx = 0;
            if (auto* sp = dynamic_cast<StreetProperty*>(prop)) {
                buildingIdx = static_cast<int>(sp->getBuildingState());
            }

            ofs << "PROPERTY " << escapeField(prop->getCode())
                << "|" << propertyTypeToString(prop->getType())
                << "|" << escapeField(ownerName)
                << "|" << propStatusToString(prop->getStatus())
                << "|" << prop->getFestivalMultiplier()
                << "|" << prop->getFestivalDuration()
                << "|" << buildingToken(buildingIdx)
                << "\n";
        }
    }

    ofs.close();

    if (!saveDeckState(filepath)) {
        if (gui) gui->showMessage("Gagal menyimpan data deck kartu.");
        return false;
    }

    std::ofstream endOfs(buildStateFilepath(filepath), std::ios::app);
    if (!endOfs.is_open()) {
        if (gui) gui->showMessage("Gagal menyelesaikan proses penyimpanan.");
        return false;
    }
    endOfs << "END\n";
    endOfs.close();

    if (!saveConfigSnapshot(filepath)) {
        if (gui) gui->showMessage("Gagal menyalin file konfigurasi ke folder data simpan.");
        return false;
    }
    if (!saveLogFile(filepath)) {
        if (gui) gui->showMessage("Gagal menyimpan log permainan: " + buildLogFilepath(filepath));
        return false;
    }
    return true;
}

bool SaveLoadManager::saveLogSnapshot(const std::string& filepath) const {
    return saveLogFile(filepath);
}

std::vector<std::string> SaveLoadManager::splitBy(const std::string& s, char sep) {
    return splitRaw(s, sep);
}

std::string SaveLoadManager::afterEq(const std::string& kv) {
    auto pos = kv.find('=');
    if (pos == std::string::npos) return "";
    return kv.substr(pos + 1);
}

bool SaveLoadManager::load(const std::string& filepath) {
    if (game == nullptr) return false;

    std::ifstream ifs(buildStateFilepath(filepath));
    if (!ifs.is_open()) {
        ifs.open(filepath);
    }
    if (!ifs.is_open()) {
        if (gui) gui->showMessage("Data simpan tidak ditemukan: " + buildStateFilepath(filepath));
        return false;
    }

    std::string line;
    if (!std::getline(ifs, line) || line.rfind("NIMONSPOLI_SAVE", 0) != 0) {
        if (gui) gui->showMessage("Gagal memuat data simpan. Format file tidak dikenali.");
        return false;
    }

    int saveVersion = 1;
    {
        std::istringstream header(line);
        std::string magic;
        header >> magic >> saveVersion;
    }

    class PendingProp {
    public:
        std::string code;
        PropertyStatus status = PropertyStatus::BANK;
        std::string ownerName = "BANK";
        int buildingIdx = 0;
        int festivalMultiplier = 1;
        int festivalDuration = 0;
    };

    class PendingHandCard {
    public:
        std::string uname;
        std::string cardToken;
    };

    class PendingDeckState {
    public:
        std::string deckType;
        std::string pileType;
        std::vector<std::string> cardTokens;
    };

    std::vector<PendingProp> pendingProps;
    std::vector<PendingHandCard> pendingHands;
    std::vector<PendingDeckState> pendingDeckStates;
    std::vector<LogEntry> inlineLogEntries;

    int savedTurn = 1;
    int savedCti = 0;
    int savedDice = 0;
    int savedOver = 0;
    int savedPhase = static_cast<int>(TurnPhase::START);
    int savedActed = 0;
    int savedShield = 0;
    std::string activeUsername;
    std::vector<std::string> pendingTurnOrderNames;

    if (saveVersion >= 2) {
        while (std::getline(ifs, line)) {
            if (line.empty() || line == "END") continue;

            std::istringstream iss(line);
            std::string head;
            iss >> head;
            std::string rest;
            std::getline(iss, rest);
            if (!rest.empty() && rest.front() == ' ') rest.erase(0, 1);

            if (head == "GAME") {
                const auto f = splitEscaped(rest, '|');
                if (f.size() >= 6) {
                    if (isIntegerToken(f[0])) savedTurn = std::stoi(f[0]);
                    activeUsername = (f.size() >= 4) ? f[3] : "";
                    if (isIntegerToken(f[4])) savedDice = std::stoi(f[4]);
                    if (isIntegerToken(f[5])) savedOver = std::stoi(f[5]);
                }
            } else if (head == "TURN") {
                const auto f = splitEscaped(rest, '|');
                if (f.size() >= 3) {
                    if (isIntegerToken(f[0])) savedPhase = std::stoi(f[0]);
                    if (isIntegerToken(f[1])) savedActed = std::stoi(f[1]);
                    if (isIntegerToken(f[2])) savedShield = std::stoi(f[2]);
                }
            } else if (head == "TURN_ORDER") {
                pendingTurnOrderNames = splitEscaped(rest, ',');
            } else if (head == "PLAYER") {
                const auto f = splitEscaped(rest, '|');
                if (f.size() < 8) continue;

                Player* p = new Player(f[0], isIntegerToken(f[1]) ? std::stoi(f[1]) : 0);
                p->setPosition(positionFromToken(f[2], game->getBoard()));
                p->setStatus(parsePlayerStatus(f[3]));

                const int consDoubles = isIntegerToken(f[4]) ? std::stoi(f[4]) : 0;
                const int jailAttempts = isIntegerToken(f[5]) ? std::stoi(f[5]) : 0;
                const int rolled = isIntegerToken(f[6]) ? std::stoi(f[6]) : 0;
                const int skillUsed = isIntegerToken(f[7]) ? std::stoi(f[7]) : 0;
                const int pendingFestival = (f.size() >= 9 && isIntegerToken(f[8])) ? std::stoi(f[8]) : 0;

                for (int i = 0; i < consDoubles; ++i) p->incrementConsecutiveDoubles();
                for (int i = 0; i < jailAttempts; ++i) p->incrementJailAttempts();
                if (rolled) p->markRolled();
                if (skillUsed) p->markSkillUsed();
                p->setPendingFestival(pendingFestival != 0);
                game->addPlayer(p);
            } else if (head == "CARD") {
                const auto f = splitEscaped(rest, '|');
                if (f.size() < 3) continue;
                PendingHandCard card;
                card.uname = f[0];
                card.cardToken = f[1] + ":" + f[2] + ":" + ((f.size() >= 4) ? f[3] : "0");
                pendingHands.push_back(std::move(card));
            } else if (head == "PROPERTY") {
                const auto f = splitEscaped(rest, '|');
                if (f.size() < 6) continue;

                PendingProp prop;
                prop.code = f[0];
                prop.ownerName = f[2];
                prop.status = parsePropStatus(f[3]);
                prop.festivalMultiplier = isIntegerToken(f[4]) ? std::stoi(f[4]) : 1;
                prop.festivalDuration = isIntegerToken(f[5]) ? std::stoi(f[5]) : 0;
                prop.buildingIdx = (f.size() >= 7) ? parseBuildingToken(f[6]) : 0;
                pendingProps.push_back(std::move(prop));
            } else if (head == "DECK") {
                const auto f = splitEscaped(rest, '|');
                if (f.size() < 3) continue;

                PendingDeckState pending;
                pending.deckType = f[0];
                pending.pileType = f[1];
                if (!f[2].empty()) pending.cardTokens = splitEscaped(f[2], ',');
                pendingDeckStates.push_back(std::move(pending));
            } else if (head == "LOG") {
                const auto f = splitEscaped(rest, '|');
                if (f.size() < 4 || !isIntegerToken(f[0])) continue;
                inlineLogEntries.emplace_back(std::stoi(f[0]), f[1], f[2], f[3]);
            }
        }
    } else {
        while (std::getline(ifs, line)) {
            if (line.empty() || line == "END") continue;

            std::istringstream iss(line);
            std::string head;
            iss >> head;
            std::string rest;
            std::getline(iss, rest);
            if (!rest.empty() && rest.front() == ' ') rest.erase(0, 1);

            if (head == "META") {
                std::istringstream kvs(rest);
                std::string kv;
                while (kvs >> kv) {
                    if      (kv.rfind("turn=", 0) == 0)             savedTurn = std::stoi(afterEq(kv));
                    else if (kv.rfind("currentTurnIndex=", 0) == 0) savedCti  = std::stoi(afterEq(kv));
                    else if (kv.rfind("lastDiceTotal=", 0) == 0)    savedDice = std::stoi(afterEq(kv));
                    else if (kv.rfind("gameOver=", 0) == 0)         savedOver = std::stoi(afterEq(kv));
                    else if (kv.rfind("phase=", 0) == 0)            savedPhase = std::stoi(afterEq(kv));
                    else if (kv.rfind("acted=", 0) == 0)            savedActed = std::stoi(afterEq(kv));
                    else if (kv.rfind("shield=", 0) == 0)           savedShield = std::stoi(afterEq(kv));
                }
            } else if (head == "TURN_ORDER") {
                auto toks = splitBy(rest, ',');
                std::vector<int> order;
                for (auto& t : toks) {
                    try { order.push_back(std::stoi(t)); } catch (...) {}
                }
                game->setTurnOrder(order);
            } else if (head == "PLAYER") {
                auto f = splitBy(rest, '|');
                if (f.size() < 8) continue;

                Player* p = new Player(f[0], std::stoi(f[1]));
                p->setPosition(std::stoi(f[2]));
                p->setStatus(parsePlayerStatus(f[3]));
                for (int i = 0; i < std::stoi(f[4]); ++i) p->incrementConsecutiveDoubles();
                for (int i = 0; i < std::stoi(f[5]); ++i) p->incrementJailAttempts();
                if (std::stoi(f[6])) p->markRolled();
                if (std::stoi(f[7])) p->markSkillUsed();
                p->setPendingFestival((f.size() >= 9) ? (std::stoi(f[8]) != 0) : false);
                game->addPlayer(p);
            } else if (head == "HAND") {
                auto f = splitBy(rest, '|');
                if (f.size() < 2) continue;

                const auto cardNames = splitBy(f[1], ',');
                for (const std::string& cardName : cardNames) {
                    PendingHandCard card;
                    card.uname = f[0];
                    card.cardToken = cardName + ":0:0";
                    pendingHands.push_back(std::move(card));
                }
            } else if (head == "PROPERTY") {
                auto f = splitBy(rest, '|');
                if (f.size() < 4) continue;

                PendingProp prop;
                prop.code = f[0];
                prop.status = parsePropStatus(f[1]);
                prop.ownerName = (f[2] == "-") ? "BANK" : f[2];
                prop.buildingIdx = parseBuildingState(f[3]);
                prop.festivalMultiplier = (f.size() >= 5) ? std::stoi(f[4]) : 1;
                prop.festivalDuration = (f.size() >= 6) ? std::stoi(f[5]) : 0;
                pendingProps.push_back(std::move(prop));
            } else if (head == "LOG") {
                auto f = splitBy(rest, '|');
                if (f.size() < 4) continue;
                try {
                    inlineLogEntries.emplace_back(std::stoi(f[0]), f[1], f[2], f[3]);
                } catch (...) {
                }
            } else if (head == "DECK") {
                auto f = splitBy(rest, '|');
                if (f.size() < 3) continue;

                PendingDeckState pending;
                pending.deckType = f[0];
                pending.pileType = f[1];
                if (!f[2].empty()) pending.cardTokens = splitBy(f[2], ',');
                pendingDeckStates.push_back(std::move(pending));
            }
        }
    }

    if (saveVersion >= 2) {
        if (!pendingTurnOrderNames.empty()) {
            std::vector<int> order;
            order.reserve(pendingTurnOrderNames.size());
            for (const std::string& username : pendingTurnOrderNames) {
                int idx = findPlayerIndex(game->getPlayers(), username);
                if (idx >= 0) order.push_back(idx);
            }
            if (!order.empty()) game->setTurnOrder(order);
        }

        if (!activeUsername.empty()) {
            const auto& order = game->getTurnOrder();
            if (!order.empty()) {
                for (std::size_t i = 0; i < order.size(); ++i) {
                    Player* player = game->getPlayer(order[i]);
                    if (player != nullptr && player->getUsername() == activeUsername) {
                        savedCti = static_cast<int>(i);
                        break;
                    }
                }
            } else {
                const int idx = findPlayerIndex(game->getPlayers(), activeUsername);
                if (idx >= 0) savedCti = idx;
            }
        }
    }

    Board* board = game->getBoard();
    if (board != nullptr) {
        for (const auto& pp : pendingProps) {
            Tile* t = nullptr;
            try { t = board->getTile(pp.code); } catch (...) { continue; }
            auto* pt = dynamic_cast<PropertyTile*>(t);
            if (pt == nullptr) continue;
            Property* prop = pt->getProperty();
            if (prop == nullptr) continue;

            Player* owner = (pp.ownerName == "BANK" || pp.ownerName == "-") ? nullptr : findPlayerByUsername(pp.ownerName);
            if (owner != nullptr) {
                prop->setOwner(owner);
                owner->addProperty(prop);
            }
            prop->setStatus(pp.status);

            if (auto* sp = dynamic_cast<StreetProperty*>(prop)) {
                const PropertyStatus originalStatus = pp.status;
                if (pp.buildingIdx > 0 && originalStatus == PropertyStatus::MORTGAGED) {
                    prop->setStatus(PropertyStatus::OWNED);
                }
                if (pp.buildingIdx >= 1 && pp.buildingIdx <= 4) {
                    for (int i = 0; i < pp.buildingIdx; ++i) sp->buildHouse();
                } else if (pp.buildingIdx == 5) {
                    for (int i = 0; i < 4; ++i) sp->buildHouse();
                    sp->buildHotel();
                }
                prop->setStatus(originalStatus);
            }

            prop->setFestivalState(pp.festivalMultiplier, pp.festivalDuration);
        }
    }

    for (const auto& ph : pendingHands) {
        Player* player = findPlayerByUsername(ph.uname);
        if (player == nullptr) continue;

        SkillCard* card = decodeSkillCardToken(ph.cardToken);
        if (card != nullptr) {
            player->addCard(card);
        }
    }

    if (logger != nullptr) {
        std::vector<LogEntry> loadedEntries;
        if (!loadLogFile(filepath, loadedEntries)) {
            loadedEntries = std::move(inlineLogEntries);
        }
        logger->loadEntries(std::move(loadedEntries));
    }

    auto loadChanceDeck = [&pendingDeckStates, this]() {
        auto* deck = game->getChanceDeck();
        if (deck == nullptr) return;

        std::vector<std::string> drawTokens;
        std::vector<std::string> discardTokens;
        for (const auto& pending : pendingDeckStates) {
            if (pending.deckType != "CHANCE") continue;
            if (pending.pileType == "DRAW") drawTokens = pending.cardTokens;
            if (pending.pileType == "DISCARD") discardTokens = pending.cardTokens;
        }
        if (drawTokens.empty() && discardTokens.empty()) return;
        if (std::all_of(drawTokens.begin(), drawTokens.end(),
                        [](const std::string& token) { return token.find(':') == std::string::npos; }) &&
            std::all_of(discardTokens.begin(), discardTokens.end(),
                        [](const std::string& token) { return token.find(':') == std::string::npos; })) {
            return;
        }

        std::vector<ChanceCard*> drawPile;
        std::vector<ChanceCard*> discardPile;
        for (const std::string& token : drawTokens) {
            if (auto* card = decodeChanceCardToken(token); card != nullptr) drawPile.push_back(card);
        }
        for (const std::string& token : discardTokens) {
            if (auto* card = decodeChanceCardToken(token); card != nullptr) discardPile.push_back(card);
        }
        deck->loadState(std::move(drawPile), std::move(discardPile));
    };

    auto loadCommunityDeck = [&pendingDeckStates, this]() {
        auto* deck = game->getCommunityDeck();
        if (deck == nullptr) return;

        std::vector<std::string> drawTokens;
        std::vector<std::string> discardTokens;
        for (const auto& pending : pendingDeckStates) {
            if (pending.deckType != "COMMUNITY") continue;
            if (pending.pileType == "DRAW") drawTokens = pending.cardTokens;
            if (pending.pileType == "DISCARD") discardTokens = pending.cardTokens;
        }
        if (drawTokens.empty() && discardTokens.empty()) return;
        if (std::all_of(drawTokens.begin(), drawTokens.end(),
                        [](const std::string& token) { return token.find(':') == std::string::npos; }) &&
            std::all_of(discardTokens.begin(), discardTokens.end(),
                        [](const std::string& token) { return token.find(':') == std::string::npos; })) {
            return;
        }

        std::vector<CommunityChestCard*> drawPile;
        std::vector<CommunityChestCard*> discardPile;
        for (const std::string& token : drawTokens) {
            if (auto* card = decodeCommunityCardToken(token); card != nullptr) drawPile.push_back(card);
        }
        for (const std::string& token : discardTokens) {
            if (auto* card = decodeCommunityCardToken(token); card != nullptr) discardPile.push_back(card);
        }
        deck->loadState(std::move(drawPile), std::move(discardPile));
    };

    auto loadSkillDeck = [&pendingDeckStates, this]() {
        auto* deck = game->getSkillDeck();
        if (deck == nullptr) return;

        std::vector<std::string> drawTokens;
        std::vector<std::string> discardTokens;
        for (const auto& pending : pendingDeckStates) {
            if (pending.deckType != "SKILL") continue;
            if (pending.pileType == "DRAW") drawTokens = pending.cardTokens;
            if (pending.pileType == "DISCARD") discardTokens = pending.cardTokens;
        }
        if (drawTokens.empty() && discardTokens.empty()) return;

        std::vector<SkillCard*> drawPile;
        std::vector<SkillCard*> discardPile;
        for (const std::string& token : drawTokens) {
            if (auto* card = decodeSkillCardToken(token); card != nullptr) drawPile.push_back(card);
        }
        for (const std::string& token : discardTokens) {
            if (auto* card = decodeSkillCardToken(token); card != nullptr) discardPile.push_back(card);
        }
        deck->loadState(std::move(drawPile), std::move(discardPile));
    };

    loadChanceDeck();
    loadCommunityDeck();
    loadSkillDeck();

    game->setCurrentTurn(savedTurn);
    game->setCurrentTurnIndex(savedCti);
    game->setLastDiceTotal(savedDice);
    game->setGameOver(savedOver != 0);
    if (turnManager != nullptr) {
        turnManager->setPhase(static_cast<TurnPhase>(savedPhase));
        turnManager->setShieldActive(savedShield != 0);
        if (savedActed != 0) {
            turnManager->markActed();
        }
    }

    return true;
}
