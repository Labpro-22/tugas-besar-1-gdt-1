#ifndef BANKRUPTCYMANAGER_HPP
#define BANKRUPTCYMANAGER_HPP

#include "core/Game.hpp"
#include "core/AuctionManager.hpp"
#include "views/IGUI.hpp"
#include "utils/data/TransactionLogger.hpp"
#include "models/Player/Player.hpp"
#include "models/Property/Property.hpp"

class BankruptcyManager {
private:
    Game* game;
    TransactionLogger* logger;
    IGUI* gui;
    AuctionManager* auctionManager;

    int  calculateMaxLiquidation(const Player& player) const;
    bool canCoverDebt(const Player& player, int amount) const;
    bool runLiquidationPanel(Player& player, int targetAmount);
    int  sellPropertyToBank(Player& player, Property* property);
    int  mortgageProperty(Player& player, Property* property);
    void declareBankruptcy(Player& debtor, Player* creditor);
    void transferAssetsToPlayer(Player& from, Player& to);
    void returnAssetsToBank(Player& player);

public:
    BankruptcyManager(Game* game, TransactionLogger* logger,
                      IGUI* gui, AuctionManager* auctionManager);

    bool handleInsufficientFunds(Player& debtor, int amount, Player* creditor);
};

#endif
