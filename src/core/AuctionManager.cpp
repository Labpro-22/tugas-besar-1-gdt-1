#include "core/AuctionManager.hpp"
#include <algorithm>
#include <set>

AuctionManager::AuctionManager(Game* game, TransactionLogger* logger, IGUI* gui)
    : game(game), logger(logger), gui(gui) {}

std::vector<Player*> AuctionManager::buildAuctionOrder(Player* triggeringPlayer) const {
    std::vector<Player*> all = game->getActivePlayers();
    if (all.empty()) return all;

    int startIdx = 0;
    for (size_t i = 0; i < all.size(); ++i) {
        if (all[i] == triggeringPlayer) { startIdx = static_cast<int>(i); break; }
    }
    std::vector<Player*> order;
    int n = static_cast<int>(all.size());
    for (int i = 0; i < n; ++i) {
        order.push_back(all[(startIdx + i) % n]);
    }
    return order;
}

bool AuctionManager::validateBid(const Player& player, int amount, int currentHighBid) const {
    if (amount <= currentHighBid) return false;
    if (!player.canAfford(amount)) return false;
    return true;
}

std::pair<AuctionAction, int> AuctionManager::collectBidOrPass(Player& player, int currentHighBid) {
    gui->showInputPrompt(player.getUsername() +
        " bid > " + std::to_string(currentHighBid) + " atau LEWAT:");
    std::string in;
    while (!gui->shouldExit()) {
        gui->update(); gui->display();
        std::string c = gui->getCommand();
        if (!c.empty() && c != "NULL") { in = c; break; }
    }
    std::string up = in;
    std::transform(up.begin(), up.end(), up.begin(),
                   [](unsigned char ch){ return std::toupper(ch); });
    if (up == "LEWAT" || up == "PASS") {
        return {AuctionAction::PASS, 0};
    }
    try {
        int bid = std::stoi(in);
        return {AuctionAction::BID, bid};
    } catch (...) {
        return {AuctionAction::PASS, 0};
    }
}

void AuctionManager::finalizeAuction(Player* winner, Property* property, int bidAmount) {
    if (winner == nullptr || bidAmount <= 0) {
        gui->showMessage("Lelang " + property->getName() + " tanpa pemenang.");
        return;
    }
    winner->deductMoney(bidAmount);
    property->setOwner(winner);
    property->setStatus(PropertyStatus::OWNED);
    winner->addProperty(property);

    if (logger) logger->log(game->getCurrentTurn(), winner->getUsername(),
                            "LELANG_MENANG",
                            property->getName() + " " + std::to_string(bidAmount));
    gui->showMessage(winner->getUsername() +
        " memenangkan lelang " + property->getName() +
        " seharga " + std::to_string(bidAmount));
}

Player* AuctionManager::runAuction(Property* property, Player* triggeringPlayer) {
    if (property == nullptr) return nullptr;
    std::vector<Player*> order = buildAuctionOrder(triggeringPlayer);
    if (order.empty()) return nullptr;

    int currentBid = 0;
    Player* highBidder = nullptr;
    std::set<Player*> passed;
    int n = static_cast<int>(order.size());

    gui->renderAuction(*property, currentBid, highBidder);

    int i = 0;
    while (static_cast<int>(passed.size()) < n) {
        Player* p = order[i % n];
        if (passed.count(p) == 0) {
            auto [act, amt] = collectBidOrPass(*p, currentBid);
            if (act == AuctionAction::PASS) {
                passed.insert(p);
            } else if (validateBid(*p, amt, currentBid)) {
                currentBid = amt;
                highBidder = p;
                gui->renderAuction(*property, currentBid, highBidder);
            } else {
                gui->showMessage("Bid tidak valid.");
                passed.insert(p);
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
