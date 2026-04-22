#include "core/BankruptcyManager.hpp"
#include "models/Property/StreetProperty.hpp"
#include <algorithm>
#include <sstream>

BankruptcyManager::BankruptcyManager(Game* game, TransactionLogger* logger,
                                     IGUI* gui, AuctionManager* auctionManager)
    : game(game), logger(logger), gui(gui), auctionManager(auctionManager) {}

int BankruptcyManager::calculateMaxLiquidation(const Player& player) const {
    int total = player.getBalance();
    for (Property* p : player.getOwnedProperties()) {
        if (auto* sp = dynamic_cast<StreetProperty*>(p)) {
            if (sp->getBuildingState() != BuildingState::NONE) {
                total += sp->sellBuildingValue();
            }
        }
        if (!p->isMortgaged()) total += p->getMortgageValue();
    }
    return total;
}

bool BankruptcyManager::canCoverDebt(const Player& player, int amount) const {
    return calculateMaxLiquidation(player) >= amount;
}

int BankruptcyManager::sellPropertyToBank(Player& player, Property* property) {
    auto* sp = dynamic_cast<StreetProperty*>(property);
    if (sp == nullptr || sp->getBuildingState() == BuildingState::NONE) return 0;
    int val = sp->sellBuildingValue();
    sp->clearBuildings();
    player.addMoney(val);
    if (logger) logger->log(game->getCurrentTurn(), player.getUsername(),
                            "JUAL_BANGUNAN", sp->getName() + " +" + std::to_string(val));
    return val;
}

int BankruptcyManager::mortgageProperty(Player& player, Property* property) {
    if (property == nullptr || property->isMortgaged()) return 0;
    int val = property->getMortgageValue();
    property->setStatus(PropertyStatus::MORTGAGED);
    player.addMoney(val);
    if (logger) logger->log(game->getCurrentTurn(), player.getUsername(),
                            "GADAI", property->getName() + " +" + std::to_string(val));
    return val;
}

bool BankruptcyManager::runLiquidationPanel(Player& player, int targetAmount) {
    gui->renderBankruptcy(player);
    gui->showMessage(player.getUsername() + " harus membayar " +
                     std::to_string(targetAmount) + ". Melikuidasi aset...");

    // auto-sell bangunan
    auto props = player.getOwnedProperties();
    for (Property* p : props) {
        if (player.getBalance() >= targetAmount) return true;
        sellPropertyToBank(player, p);
    }
    // auto-gadai properti
    props = player.getOwnedProperties();
    for (Property* p : props) {
        if (player.getBalance() >= targetAmount) return true;
        mortgageProperty(player, p);
    }
    return player.getBalance() >= targetAmount;
}

void BankruptcyManager::transferAssetsToPlayer(Player& from, Player& to) {
    int remaining = from.getBalance();
    if (remaining > 0) {
        to.addMoney(remaining);
        from.deductMoney(remaining);
    }
    auto owned = from.getOwnedProperties();
    for (Property* p : owned) {
        from.removeProperty(p);
        p->setOwner(&to);
        p->setStatus(PropertyStatus::OWNED);
        to.addProperty(p);
    }
}

void BankruptcyManager::returnAssetsToBank(Player& player) {
    auto owned = player.getOwnedProperties();
    for (Property* p : owned) {
        player.removeProperty(p);
        p->clearOwner();
        p->setStatus(PropertyStatus::BANK);
        if (auctionManager != nullptr) {
            auctionManager->runAuction(p, &player);
        }
    }
    int rem = player.getBalance();
    if (rem > 0) player.deductMoney(rem);
}

void BankruptcyManager::declareBankruptcy(Player& debtor, Player* creditor) {
    if (creditor != nullptr) transferAssetsToPlayer(debtor, *creditor);
    else                     returnAssetsToBank(debtor);

    debtor.setStatus(PlayerStatus::BANKRUPT);
    if (logger) logger->log(game->getCurrentTurn(), debtor.getUsername(),
                            "BANGKRUT", creditor ? creditor->getUsername() : "BANK");
    gui->showMessage(debtor.getUsername() + " DINYATAKAN BANGKRUT.");
}

bool BankruptcyManager::handleInsufficientFunds(Player& debtor, int amount, Player* creditor) {
    if (amount <= 0) return true;

    if (debtor.canAfford(amount)) {
        debtor.deductMoney(amount);
        if (creditor != nullptr) creditor->addMoney(amount);
        return true;
    }

    if (!canCoverDebt(debtor, amount)) {
        declareBankruptcy(debtor, creditor);
        return false;
    }

    runLiquidationPanel(debtor, amount);

    if (debtor.canAfford(amount)) {
        debtor.deductMoney(amount);
        if (creditor != nullptr) creditor->addMoney(amount);
        return true;
    }

    declareBankruptcy(debtor, creditor);
    return false;
}
