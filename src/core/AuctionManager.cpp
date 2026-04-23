#include "core/AuctionManager.hpp"
#include <algorithm>
#include <sstream>

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

bool AuctionManager::validateBid(const Player& player, int amount, int currentHighBid,
                                 bool hasHighBidder) const {
    if (hasHighBidder) {
        if (amount <= currentHighBid) return false;
    } else {
        if (amount < currentHighBid) return false;
    }
    if (!player.canAfford(amount)) return false;
    return true;
}

std::pair<AuctionAction, int> AuctionManager::collectBidOrPass(Player& player, int currentHighBid,
                                                               bool hasHighBidder, bool forceBid) {
    gui->showMessage("Giliran lelang: " + player.getUsername());

    std::string minimumLabel = hasHighBidder ? ("> M" + std::to_string(currentHighBid))
                                             : (">= M" + std::to_string(currentHighBid));
    if (forceBid) {
        gui->showInputPrompt("Masukkan BID <jumlah> (minimal " + minimumLabel + "). Kamu wajib menawar.");
    } else {
        gui->showInputPrompt("Masukkan PASS atau BID <jumlah> (minimal " + minimumLabel + ").");
    }

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
        if (forceBid) {
            return {AuctionAction::INVALID, 0};
        }
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
    if (winner == nullptr) {
        if (logger) {
            logger->log(game->getCurrentTurn(), "BANK",
                        "LELANG",
                        property->getName() + " (" + property->getCode() + ") tanpa pemenang");
        }
        gui->showMessage("Lelang selesai tanpa pemenang.");
        gui->showMessage("Properti tetap menjadi milik Bank.");
        return;
    }
    winner->deductMoney(bidAmount);
    property->setOwner(winner);
    property->setStatus(PropertyStatus::OWNED);
    winner->addProperty(property);

    if (logger) logger->log(game->getCurrentTurn(), winner->getUsername(),
                            "LELANG",
                            "Menang " + property->getName() + " (" + property->getCode() +
                            ") seharga M" + std::to_string(bidAmount));
    gui->showMessage("Lelang selesai.");
    gui->showMessage("Pemenang lelang: " + winner->getUsername());
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
    int n = static_cast<int>(order.size());
    int consecutivePasses = 0;

    gui->showMessage("Properti " + property->getName() + " (" + property->getCode() + ") akan dilelang.");
    if (triggeringPlayer != nullptr) {
        gui->showMessage("Urutan lelang dimulai dari pemain setelah " +
                         triggeringPlayer->getUsername() + ".");
    }
    gui->renderAuction(*property, currentBid, highBidder);

    int i = 0;
    while (!gui->shouldExit()) {
        Player* p = order[i % n];
        const bool hasHighBidder = (highBidder != nullptr);
        const bool forceBid = (!hasHighBidder && consecutivePasses >= n - 1);

        auto [act, amt] = collectBidOrPass(*p, currentBid, hasHighBidder, forceBid);
        if (act == AuctionAction::INVALID) {
            if (forceBid) {
                gui->showMessage("Belum ada penawaran sama sekali. Kamu wajib memasukkan BID <jumlah>.");
            } else {
                gui->showMessage("Perintah lelang tidak valid. Gunakan PASS atau BID <jumlah>.");
            }
            continue;
        }

        if (act == AuctionAction::PASS) {
            gui->showMessage(p->getUsername() + " memilih PASS.");
            ++consecutivePasses;

            if (highBidder != nullptr && consecutivePasses >= n - 1) {
                break;
            }
        } else if (validateBid(*p, amt, currentBid, hasHighBidder)) {
            currentBid = amt;
            highBidder = p;
            consecutivePasses = 0;
            if (logger) {
                logger->log(game->getCurrentTurn(), p->getUsername(),
                            "LELANG",
                            property->getCode() + " BID M" + std::to_string(currentBid));
            }
            gui->showMessage("Penawaran tertinggi: M" + std::to_string(currentBid) +
                             " (" + highBidder->getUsername() + ")");
            gui->renderAuction(*property, currentBid, highBidder);
        } else {
            if (hasHighBidder) {
                gui->showMessage("Bid tidak valid.");
                gui->showMessage("Jumlah bid harus lebih tinggi dari penawaran saat ini dan tidak boleh melebihi saldo kamu.");
            } else {
                gui->showMessage("Bid tidak valid.");
                gui->showMessage("Penawaran awal minimal M" + std::to_string(currentBid) +
                                 " dan tidak boleh melebihi saldo kamu.");
            }
            continue;
        }

        ++i;
    }

    finalizeAuction(highBidder, property, currentBid);
    return highBidder;
}
