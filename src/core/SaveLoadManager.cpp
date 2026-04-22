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

SaveLoadManager::SaveLoadManager(Game* game, TransactionLogger* logger, IGUI* gui)
    : game(game), logger(logger), gui(gui) {}

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

bool SaveLoadManager::save(const std::string& filepath) {
    if (game == nullptr) return false;

    std::ofstream ofs(filepath);
    if (!ofs.is_open()) {
        if (gui) gui->showMessage("Gagal membuka file simpan: " + filepath);
        return false;
    }

    ofs << "NIMONSPOLI_SAVE 1\n";
    ofs << "META turn=" << game->getCurrentTurn()
        << " currentTurnIndex=" << game->getCurrentTurnIndex()
        << " lastDiceTotal=" << game->getLastDiceTotal()
        << " gameOver=" << (game->isGameOver() ? 1 : 0) << "\n";

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
                << "\n";
        }
    }

    if (logger != nullptr) {
        for (const LogEntry& e : logger->getFullLog()) {
            ofs << "LOG " << e.getTurn()
                << "|" << e.getUsername()
                << "|" << e.getActionType()
                << "|" << e.getDetail()
                << "\n";
        }
    }

    ofs << "END\n";
    ofs.close();
    return true;
}

static std::vector<std::string> splitBy(const std::string& s, char sep) {
    std::vector<std::string> out;
    std::stringstream ss(s);
    std::string tok;
    while (std::getline(ss, tok, sep)) out.push_back(tok);
    return out;
}

static std::string afterEq(const std::string& kv) {
    auto pos = kv.find('=');
    if (pos == std::string::npos) return "";
    return kv.substr(pos + 1);
}

bool SaveLoadManager::load(const std::string& filepath) {
    if (game == nullptr) return false;

    std::ifstream ifs(filepath);
    if (!ifs.is_open()) {
        if (gui) gui->showMessage("File simpan tidak ditemukan: " + filepath);
        return false;
    }

    std::string line;
    std::getline(ifs, line);
    if (line.rfind("NIMONSPOLI_SAVE", 0) != 0) {
        if (gui) gui->showMessage("Format save tidak dikenali.");
        return false;
    }

    struct PendingProp {
        std::string code;
        PropertyStatus status;
        std::string ownerName;
        int buildingIdx;
    };
    std::vector<PendingProp> pendingProps;
    std::vector<LogEntry> logEntries;

    struct PendingHand { std::string uname; std::vector<std::string> cardNames; };
    std::vector<PendingHand> pendingHands;

    int savedTurn = 1, savedCti = 0, savedDice = 0, savedOver = 0;

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

            Player* p = new Player(uname, balance);
            p->setPosition(position);
            p->setStatus(ps);
            for (int i = 0; i < consDoubles;  ++i) p->incrementConsecutiveDoubles();
            for (int i = 0; i < jailAttempts; ++i) p->incrementJailAttempts();
            if (rolled)    p->markRolled();
            if (skillUsed) p->markSkillUsed();
            game->addPlayer(p);
        } else if (head == "HAND") {
            auto f = splitBy(rest, '|');
            if (f.size() < 2) continue;
            PendingHand ph{ f[0], splitBy(f[1], ',') };
            pendingHands.push_back(ph);
        } else if (head == "PROPERTY") {
            auto f = splitBy(rest, '|');
            if (f.size() < 4) continue;
            pendingProps.push_back({ f[0], parsePropStatus(f[1]), f[2], parseBuildingState(f[3]) });
        } else if (head == "LOG") {
            auto f = splitBy(rest, '|');
            if (f.size() < 4) continue;
            try {
                logEntries.emplace_back(std::stoi(f[0]), f[1], f[2], f[3]);
            } catch (...) {}
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
        logger->loadEntries(logEntries);
    }

    game->setCurrentTurn(savedTurn);
    game->setCurrentTurnIndex(savedCti);
    game->setLastDiceTotal(savedDice);
    game->setGameOver(savedOver != 0);

    return true;
}
