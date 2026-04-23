#ifndef AUCTIONMANAGER_HPP
#define AUCTIONMANAGER_HPP

#include "core/Game.hpp"
#include "views/IGUI.hpp"
#include "utils/data/TransactionLogger.hpp"
#include "models/Player/Player.hpp"
#include "models/Property/Property.hpp"
#include <string>
#include <utility>
#include <vector>

enum class AuctionAction { BID, PASS, INVALID };

class AuctionManager {
private:
    Game* game;
    TransactionLogger* logger;
    IGUI* gui;

    std::vector<Player*> buildAuctionOrder(Player* triggeringPlayer) const;
    std::pair<AuctionAction, int> collectBidOrPass(Player& player, int currentHighBid);
    bool validateBid(const Player& player, int amount, int currentHighBid) const;
    void finalizeAuction(Player* winner, Property* property, int bidAmount);

public:
    AuctionManager(Game* game, TransactionLogger* logger, IGUI* gui);

    Player* runAuction(Property* property, Player* triggeringPlayer);
};

#endif
