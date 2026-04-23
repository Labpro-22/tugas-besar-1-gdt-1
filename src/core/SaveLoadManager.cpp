#include "core/SaveLoadManager.hpp"
#include "models/Player/Player.hpp"
#include "models/Property/Property.hpp"
#include "models/Property/StreetProperty.hpp"
#include "models/BoardAndTiles/Board.hpp"
#include "models/BoardAndTiles/Tile.hpp"
#include "models/BoardAndTiles/PropertyTile.hpp"
#include "models/CardAndDeck/SkillCard.hpp"
#include "models/CardAndDeck/CardDeck.hpp"

#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <filesystem>
#include <type_traits>

namespace {
template <typename T>
std::string joinCardNames(const std::vector<T*>& cards) {
    std::ostringstream oss;
    for (size_t i = 0; i < cards.size(); ++i) {
        if (cards[i] != nullptr) {
            oss << cards[i]->getCardName();
        }
        if (i + 1 < cards.size()) {
            oss << ",";
        }
    }
    return oss.str();
}
}

SaveLoadManager::SaveLoadManager(Game* game, TransactionLogger* logger, TurnManager* turnManager, IGUI* gui,
                                 std::string configSourceDir)
    : game(game),
      logger(logger),
      turnManager(turnManager),
      gui(gui),
      configSourceDir(std::move(configSourceDir)) {}

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
    logOfs << entries.size() << "\n";
    for (const LogEntry& entry : entries) {
        logOfs << entry.toSaveString() << "\n";
    }
    return true;
}

bool SaveLoadManager::saveConfigSnapshot(const std::string& savePath) const {
    namespace fs = std::filesystem;

    const fs::path saveDir(resolveSaveDirectory(savePath));
    const fs::path baseDir(configSourceDir.empty() ? "data/default" : configSourceDir);
    const std::vector<std::string> configFiles = {
        "aksi.txt",
        "misc.txt",
        "property.txt",
        "railroad.txt",
        "special.txt",
        "tax.txt",
        "utility.txt"
    };

    for (const std::string& file : configFiles) {
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
        ofs << "DECK CHANCE|DRAW|" << joinCardNames(deck->getDrawPile()) << "\n";
        ofs << "DECK CHANCE|DISCARD|" << joinCardNames(deck->getDiscardPile()) << "\n";
    }
    if (auto* deck = game->getCommunityDeck(); deck != nullptr) {
        ofs << "DECK COMMUNITY|DRAW|" << joinCardNames(deck->getDrawPile()) << "\n";
        ofs << "DECK COMMUNITY|DISCARD|" << joinCardNames(deck->getDiscardPile()) << "\n";
    }
    if (auto* deck = game->getSkillDeck(); deck != nullptr) {
        ofs << "DECK SKILL|DRAW|" << joinCardNames(deck->getDrawPile()) << "\n";
        ofs << "DECK SKILL|DISCARD|" << joinCardNames(deck->getDiscardPile()) << "\n";
    }

    return true;
}

bool SaveLoadManager::loadLogFile(const std::string& saveFilepath, std::vector<LogEntry>& outEntries) const {
    auto parseLogStream = [](std::istream& logIfs, std::vector<LogEntry>& parsedEntries) -> bool {
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

    std::ifstream txtIfs(buildLogFilepath(saveFilepath));
    if (txtIfs.is_open()) {
        return parseLogStream(txtIfs, outEntries);
    }

    std::ifstream legacyIfs(saveFilepath + ".log");
    if (legacyIfs.is_open()) {
        return parseLogStream(legacyIfs, outEntries);
    }

    return false;
}

bool SaveLoadManager::save(const std::string& filepath) {
    if (game == nullptr) return false;

    namespace fs = std::filesystem;
    std::error_code ec;
    fs::create_directories(resolveSaveDirectory(filepath), ec);
    if (ec) {
        if (gui) gui->showMessage("Gagal membuat folder save: " + resolveSaveDirectory(filepath));
        return false;
    }

    std::ofstream ofs(buildStateFilepath(filepath));
    if (!ofs.is_open()) {
        if (gui) gui->showMessage("Gagal membuka file simpan: " + buildStateFilepath(filepath));
        return false;
    }

    ofs << "NIMONSPOLI_SAVE 1\n";
    ofs << "META turn=" << game->getCurrentTurn()
        << " currentTurnIndex=" << game->getCurrentTurnIndex()
        << " lastDiceTotal=" << game->getLastDiceTotal()
        << " gameOver=" << (game->isGameOver() ? 1 : 0)
        << " phase=" << static_cast<int>(turnManager ? turnManager->getPhase() : TurnPhase::START)
        << " acted=" << (turnManager && turnManager->hasActedThisTurn() ? 1 : 0)
        << " shield=" << (turnManager && turnManager->isShieldActive() ? 1 : 0)
        << "\n";

    ofs << "TURN_ORDER ";
    const auto& order = game->getTurnOrder();
    for (size_t i = 0; i < order.size(); ++i) {
        ofs << order[i];
        if (i + 1 < order.size()) ofs << ",";
    }
    ofs << "\n";

    for (Player* p : game->getPlayers()) {
        ofs << "PLAYER " << p->getUsername()
            << "|" << p->getBalance()
            << "|" << p->getPosition()
            << "|" << statusToString(p->getStatus())
            << "|" << p->getConsecutiveDoubles()
            << "|" << p->getJailAttempts()
            << "|" << (p->hasRolled() ? 1 : 0)
            << "|" << (p->hasUsedSkill() ? 1 : 0)
            << "|" << (p->hasPendingFestival() ? 1 : 0)
            << "\n";

        const auto& hand = p->getHandCards();
        if (!hand.empty()) {
            ofs << "HAND " << p->getUsername() << "|";
            for (size_t i = 0; i < hand.size(); ++i) {
                ofs << hand[i]->getCardName();
                if (i + 1 < hand.size()) ofs << ",";
            }
            ofs << "\n";
        }
    }

    Board* board = game->getBoard();
    if (board != nullptr) {
        for (Tile* t : board->getAllTiles()) {
            auto* pt = dynamic_cast<PropertyTile*>(t);
            if (pt == nullptr) continue;
            Property* prop = pt->getProperty();
            if (prop == nullptr) continue;

            std::string ownerName = prop->getOwner() ? prop->getOwner()->getUsername() : "-";
            int buildingIdx = 0;
            if (auto* sp = dynamic_cast<StreetProperty*>(prop)) {
                buildingIdx = static_cast<int>(sp->getBuildingState());
            }

            ofs << "PROPERTY " << prop->getCode()
                << "|" << propStatusToString(prop->getStatus())
                << "|" << ownerName
                << "|" << buildingToString(buildingIdx)
                << "|" << prop->getFestivalMultiplier()
                << "|" << prop->getFestivalDuration()
                << "\n";
        }
    }

    ofs.close();
    if (!saveDeckState(filepath)) {
        if (gui) gui->showMessage("Gagal menyimpan state deck.");
        return false;
    }
    std::ofstream endOfs(buildStateFilepath(filepath), std::ios::app);
    if (!endOfs.is_open()) {
        if (gui) gui->showMessage("Gagal menyelesaikan file simpan.");
        return false;
    }
    endOfs << "END\n";
    endOfs.close();
    if (!saveConfigSnapshot(filepath)) {
        if (gui) gui->showMessage("Gagal menyalin file konfigurasi ke folder save.");
        return false;
    }
    if (!saveLogFile(filepath)) {
        if (gui) gui->showMessage("Gagal menyimpan file log: " + buildLogFilepath(filepath));
        return false;
    }
    return true;
}

bool SaveLoadManager::saveLogSnapshot(const std::string& filepath) const {
    return saveLogFile(filepath);
}

std::vector<std::string> SaveLoadManager::splitBy(const std::string& s, char sep) {
    std::vector<std::string> out;
    std::stringstream ss(s);
    std::string tok;
    while (std::getline(ss, tok, sep)) out.push_back(tok);
    return out;
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
        if (gui) gui->showMessage("File simpan tidak ditemukan: " + buildStateFilepath(filepath));
        return false;
    }

    std::string line;
    std::getline(ifs, line);
    if (line.rfind("NIMONSPOLI_SAVE", 0) != 0) {
        if (gui) gui->showMessage("Format save tidak dikenali.");
        return false;
    }

    class PendingProp {
    public:
        std::string code;
        PropertyStatus status;
        std::string ownerName;
        int buildingIdx;
        int festivalMultiplier;
        int festivalDuration;
        PendingProp() : status(PropertyStatus::BANK), buildingIdx(0), festivalMultiplier(1), festivalDuration(0) {}
        PendingProp(std::string c, PropertyStatus s, std::string o, int b, int m, int d)
            : code(std::move(c)), status(s), ownerName(std::move(o)), buildingIdx(b),
              festivalMultiplier(m), festivalDuration(d) {}
    };
    std::vector<PendingProp> pendingProps;
    std::vector<LogEntry> logEntries;

    class PendingHand {
    public:
        std::string uname;
        std::vector<std::string> cardNames;
        PendingHand() = default;
        PendingHand(std::string u, std::vector<std::string> c)
            : uname(std::move(u)), cardNames(std::move(c)) {}
    };
    std::vector<PendingHand> pendingHands;

    class PendingDeckState {
    public:
        std::string deckType;
        std::string pileType;
        std::vector<std::string> cardNames;
    };
    std::vector<PendingDeckState> pendingDeckStates;

    int savedTurn = 1, savedCti = 0, savedDice = 0, savedOver = 0;
    int savedPhase = static_cast<int>(TurnPhase::START);
    int savedActed = 0;
    int savedShield = 0;

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
            std::string uname   = f[0];
            int balance         = std::stoi(f[1]);
            int position        = std::stoi(f[2]);
            PlayerStatus ps     = parsePlayerStatus(f[3]);
            int consDoubles     = std::stoi(f[4]);
            int jailAttempts    = std::stoi(f[5]);
            int rolled          = std::stoi(f[6]);
            int skillUsed       = std::stoi(f[7]);
            int pendingFestival = (f.size() >= 9) ? std::stoi(f[8]) : 0;

            Player* p = new Player(uname, balance);
            p->setPosition(position);
            p->setStatus(ps);
            for (int i = 0; i < consDoubles;  ++i) p->incrementConsecutiveDoubles();
            for (int i = 0; i < jailAttempts; ++i) p->incrementJailAttempts();
            if (rolled)    p->markRolled();
            if (skillUsed) p->markSkillUsed();
            p->setPendingFestival(pendingFestival != 0);
            game->addPlayer(p);
        } else if (head == "HAND") {
            auto f = splitBy(rest, '|');
            if (f.size() < 2) continue;
            PendingHand ph{ f[0], splitBy(f[1], ',') };
            pendingHands.push_back(ph);
        } else if (head == "PROPERTY") {
            auto f = splitBy(rest, '|');
            if (f.size() < 4) continue;
            pendingProps.push_back({
                f[0],
                parsePropStatus(f[1]),
                f[2],
                parseBuildingState(f[3]),
                (f.size() >= 5) ? std::stoi(f[4]) : 1,
                (f.size() >= 6) ? std::stoi(f[5]) : 0
            });
        } else if (head == "LOG") {
            auto f = splitBy(rest, '|');
            if (f.size() < 4) continue;
            try {
                logEntries.emplace_back(std::stoi(f[0]), f[1], f[2], f[3]);
            } catch (...) {}
        } else if (head == "DECK") {
            auto f = splitBy(rest, '|');
            if (f.size() < 3) continue;
            PendingDeckState pending;
            pending.deckType = f[0];
            pending.pileType = f[1];
            if (!f[2].empty()) {
                pending.cardNames = splitBy(f[2], ',');
            }
            pendingDeckStates.push_back(std::move(pending));
        }
    }

    // Apply property overlay setelah semua player dibuat
    Board* board = game->getBoard();
    if (board != nullptr) {
        for (const auto& pp : pendingProps) {
            Tile* t = nullptr;
            try { t = board->getTile(pp.code); } catch (...) { continue; }
            auto* pt = dynamic_cast<PropertyTile*>(t);
            if (pt == nullptr) continue;
            Property* prop = pt->getProperty();
            if (prop == nullptr) continue;

            Player* owner = (pp.ownerName == "-") ? nullptr : findPlayerByUsername(pp.ownerName);
            if (owner != nullptr) {
                prop->setOwner(owner);
                owner->addProperty(prop);
            }
            prop->setStatus(pp.status);

            if (auto* sp = dynamic_cast<StreetProperty*>(prop)) {
                // pastikan status OWNED untuk bisa buildHouse
                PropertyStatus orig = pp.status;
                if (pp.buildingIdx > 0 && orig == PropertyStatus::MORTGAGED) {
                    prop->setStatus(PropertyStatus::OWNED);
                }
                int target = pp.buildingIdx;
                if (target >= 1 && target <= 4) {
                    for (int i = 0; i < target; ++i) sp->buildHouse();
                } else if (target == 5) {
                    for (int i = 0; i < 4; ++i) sp->buildHouse();
                    sp->buildHotel();
                }
                prop->setStatus(orig);
            }

            prop->setFestivalState(pp.festivalMultiplier, pp.festivalDuration);
        }
    }

    // Kembalikan kartu skill dari deck ke tangan pemain
    auto* skillDeck = game->getSkillDeck();
    if (skillDeck != nullptr) {
        for (const auto& ph : pendingHands) {
            Player* p = findPlayerByUsername(ph.uname);
            if (p == nullptr) continue;
            for (const std::string& cname : ph.cardNames) {
                SkillCard* card = skillDeck->takeByName(cname);
                if (card != nullptr) p->addCard(card);
            }
        }
    }

    if (logger != nullptr) {
        std::vector<LogEntry> loadedEntries;
        if (!loadLogFile(filepath, loadedEntries)) {
            loadedEntries = std::move(logEntries);
        }
        logger->loadEntries(std::move(loadedEntries));
    }

    auto applyDeckState = [&pendingDeckStates](auto* deck, const std::string& deckType) {
        if (deck == nullptr) return;

        std::vector<std::string> drawNames;
        std::vector<std::string> discardNames;
        for (const auto& pending : pendingDeckStates) {
            if (pending.deckType != deckType) continue;
            if (pending.pileType == "DRAW") {
                drawNames = pending.cardNames;
            } else if (pending.pileType == "DISCARD") {
                discardNames = pending.cardNames;
            }
        }

        if (drawNames.empty() && discardNames.empty()) return;

        using CardPtr = typename std::decay_t<decltype(deck->getDrawPile())>::value_type;
        std::vector<CardPtr> drawPile;
        std::vector<CardPtr> discardPile;
        for (const std::string& name : drawNames) {
            if (CardPtr card = deck->takeByName(name); card != nullptr) {
                drawPile.push_back(card);
            }
        }
        for (const std::string& name : discardNames) {
            if (CardPtr card = deck->takeByName(name); card != nullptr) {
                discardPile.push_back(card);
            }
        }
        deck->loadState(std::move(drawPile), std::move(discardPile));
    };

    applyDeckState(game->getChanceDeck(), "CHANCE");
    applyDeckState(game->getCommunityDeck(), "COMMUNITY");
    applyDeckState(game->getSkillDeck(), "SKILL");

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
