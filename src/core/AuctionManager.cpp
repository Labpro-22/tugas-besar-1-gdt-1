#include "core/AuctionManager.hpp"
#include <algorithm>
#include <sstream>
#include <set>

AuctionManager::AuctionManager(Game* game, TransactionLogger* logger, IGUI* gui)
    : game(game), logger(logger), gui(gui) {}

std::vector<Player*> AuctionManager::buildAuctionOrder(Player* triggeringPlayer) const {
    std::vector<Player*> orderedPlayers;
    if (game == nullptr) return orderedPlayers;

    const auto& players = game->getPlayers();
    const auto& turnOrder = game->getTurnOrder();
    int startIdx = 0;

    std::vector<Player*> fullTurnOrder;
    if (!turnOrder.empty()) {
        for (int idx : turnOrder) {
            if (idx >= 0 && idx < static_cast<int>(players.size()) && players[idx] != nullptr) {
                fullTurnOrder.push_back(players[idx]);
            }
        }
    } else {
        for (Player* player : players) {
            if (player != nullptr) {
                fullTurnOrder.push_back(player);
            }
        }
    }

    if (fullTurnOrder.empty()) return orderedPlayers;

    if (triggeringPlayer != nullptr) {
        for (size_t i = 0; i < fullTurnOrder.size(); ++i) {
            if (fullTurnOrder[i] == triggeringPlayer) {
                startIdx = static_cast<int>((i + 1) % fullTurnOrder.size());
                break;
            }
        }
    }

    int n = static_cast<int>(fullTurnOrder.size());
    for (int i = 0; i < n; ++i) {
        Player* candidate = fullTurnOrder[(startIdx + i) % n];
        if (candidate != nullptr && candidate->getStatus() != PlayerStatus::BANKRUPT) {
            orderedPlayers.push_back(candidate);
        }
    }
    return orderedPlayers;
}

bool AuctionManager::validateBid(const Player& player, int amount, int currentHighBid) const {
    if (amount <= currentHighBid) return false;
    if (!player.canAfford(amount)) return false;
    return true;
}

std::pair<AuctionAction, int> AuctionManager::collectBidOrPass(Player& player, int currentHighBid) {
    gui->showMessage("Giliran: " + player.getUsername());
    gui->showInputPrompt("Aksi (PASS / BID <jumlah>) [min > M" +
                         std::to_string(currentHighBid) + "]");
    std::string in;
    while (!gui->shouldExit()) {
        gui->update(); gui->display();
        std::string c = gui->getCommand();
        if (!c.empty() && c != "NULL") { in = c; break; }
    }
    std::istringstream iss(in);
    std::string action;
    iss >> action;
    std::transform(action.begin(), action.end(), action.begin(),
                   [](unsigned char ch){ return std::toupper(ch); });

    if (action == "LEWAT" || action == "PASS") {
        return {AuctionAction::PASS, 0};
    }

    if (action == "BID") {
        int bid = 0;
        if (iss >> bid) {
            return {AuctionAction::BID, bid};
        }
        return {AuctionAction::INVALID, 0};
    }

    try {
        int bid = std::stoi(in);
        return {AuctionAction::BID, bid};
    } catch (...) {
        return {AuctionAction::INVALID, 0};
    }
}

void AuctionManager::finalizeAuction(Player* winner, Property* property, int bidAmount) {
    if (winner == nullptr || bidAmount <= 0) {
        gui->showMessage("Lelang selesai tanpa pemenang. Properti tetap milik Bank.");
        return;
    }
    winner->deductMoney(bidAmount);
    property->setOwner(winner);
    property->setStatus(PropertyStatus::OWNED);
    winner->addProperty(property);

    if (logger) logger->log(game->getCurrentTurn(), winner->getUsername(),
                            "LELANG_MENANG",
                            property->getName() + " " + std::to_string(bidAmount));
    gui->showMessage("Lelang selesai!");
    gui->showMessage("Pemenang: " + winner->getUsername());
    gui->showMessage("Harga akhir: M" + std::to_string(bidAmount));
    gui->showMessage("Properti " + property->getName() + " (" + property->getCode() +
                     ") kini dimiliki " + winner->getUsername() + ".");
}

Player* AuctionManager::runAuction(Property* property, Player* triggeringPlayer) {
    if (property == nullptr) return nullptr;
    std::vector<Player*> order = buildAuctionOrder(triggeringPlayer);
    if (order.empty()) return nullptr;

    int currentBid = 0;
    Player* highBidder = nullptr;
    std::set<Player*> passed;
    int n = static_cast<int>(order.size());

    gui->showMessage("Properti " + property->getName() + " (" + property->getCode() + ") akan dilelang!");
    if (triggeringPlayer != nullptr) {
        gui->showMessage("Urutan lelang dimulai dari pemain setelah " +
                         triggeringPlayer->getUsername() + ".");
    }
    gui->renderAuction(*property, currentBid, highBidder);

    int i = 0;
    while (static_cast<int>(passed.size()) < n) {
        Player* p = order[i % n];
        if (passed.count(p) == 0) {
            auto [act, amt] = collectBidOrPass(*p, currentBid);
            if (act == AuctionAction::PASS) {
                gui->showMessage(p->getUsername() + " PASS");
                passed.insert(p);
            } else if (validateBid(*p, amt, currentBid)) {
                currentBid = amt;
                highBidder = p;
                gui->showMessage("Penawaran tertinggi: M" + std::to_string(currentBid) +
                                 " (" + highBidder->getUsername() + ")");
                gui->renderAuction(*property, currentBid, highBidder);
            } else if (act == AuctionAction::INVALID) {
                gui->showMessage("Input tidak valid. Gunakan PASS atau BID <jumlah>.");
                continue;
            } else {
                gui->showMessage("Bid tidak valid. Jumlah harus melebihi penawaran tertinggi dan saldo kamu.");
                continue;
            }
        }
        ++i;
        if (highBidder != nullptr &&
            static_cast<int>(passed.size()) == n - 1 &&
            passed.count(highBidder) == 0) {
            break;
        }
    }

    finalizeAuction(highBidder, property, currentBid);
    return highBidder;
}
