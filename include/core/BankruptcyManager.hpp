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
    class LiquidationOption;
    Game* game;
    TransactionLogger* logger;
    IGUI* gui;
    AuctionManager* auctionManager;

    int  calculateMaxLiquidation(const Player& player) const;
    bool canCoverDebt(const Player& player, int amount) const;
    bool runLiquidationPanel(Player& player, int targetAmount);
    int  calculateSaleValue(const Property* property) const;
    int  calculateMortgageValue(const Property* property) const;
    int  sellPropertyToBank(Player& player, Property* property);
    int  mortgageProperty(Player& player, Property* property);
    void declareBankruptcy(Player& debtor, Player* creditor);
    void transferAssetsToPlayer(Player& from, Player& to);
    void returnAssetsToBank(Player& player);
    std::vector<LiquidationOption> buildSellOptions(const Player& player) const;
    std::vector<LiquidationOption> buildMortgageOptions(const Player& player) const;
    std::vector<LiquidationOption> buildEstimatePlan(const Player& player) const;
    void showLiquidationEstimate(const Player& debtor, int amount, Player* creditor,
                                 const std::string& obligationLabel) const;
    static std::string waitForInput(IGUI* gui, const std::string& prompt);

public:
    BankruptcyManager(Game* game, TransactionLogger* logger,
                      IGUI* gui, AuctionManager* auctionManager);

    bool handleInsufficientFunds(Player& debtor, int amount, Player* creditor,
                                 const std::string& obligationLabel = "");
};

#endif
