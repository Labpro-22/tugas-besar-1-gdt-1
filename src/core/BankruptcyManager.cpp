#include "core/BankruptcyManager.hpp"

#include "models/Property/StreetProperty.hpp"
#include "utils/Formatter.hpp"

#include <algorithm>
#include <sstream>

namespace {
std::string propertyColorLabel(const Property* property) {
    if (property == nullptr || !property->isStreet()) {
        if (property != nullptr && property->getType() == PropertyType::RAILROAD) return "STASIUN";
        if (property != nullptr && property->getType() == PropertyType::UTILITY) return "UTILITAS";
        return "DEFAULT";
    }

    auto* street = static_cast<const StreetProperty*>(property);
    const std::string color = street->getColorGroup();
    if (color == "BIRU_MUDA") return "BIRU MUDA";
    if (color == "MERAH_MUDA") return "PINK";
    if (color == "BIRU_TUA") return "BIRU TUA";
    return color;
}

std::string buildingLabel(const StreetProperty* street) {
    if (street == nullptr) return "";

    switch (street->getBuildingState()) {
        case BuildingState::HOUSE_1: return "1 rumah";
        case BuildingState::HOUSE_2: return "2 rumah";
        case BuildingState::HOUSE_3: return "3 rumah";
        case BuildingState::HOUSE_4: return "4 rumah";
        case BuildingState::HOTEL:   return "Hotel";
        default:                     return "";
    }
}

std::string ownershipStatusLabel(const Property* property) {
    if (property == nullptr) return "BANK";

    if (property->getStatus() == PropertyStatus::MORTGAGED) {
        return "MORTGAGED [M]";
    }

    std::string status = "OWNED";
    if (property->isStreet()) {
        auto* street = static_cast<const StreetProperty*>(property);
        std::string building = buildingLabel(street);
        if (!building.empty()) {
            status += " (" + building + ")";
        }
    }
    return status;
}
}

class BankruptcyManager::LiquidationOption {
public:
    enum class Kind { SELL, MORTGAGE };

    Kind kind;
    Property* property;
    int value;

    LiquidationOption(Kind kind, Property* property, int value)
        : kind(kind), property(property), value(value) {}
};

BankruptcyManager::BankruptcyManager(Game* game, TransactionLogger* logger,
                                     IGUI* gui, AuctionManager* auctionManager)
    : game(game), logger(logger), gui(gui), auctionManager(auctionManager) {}

std::string BankruptcyManager::waitForInput(IGUI* gui, const std::string& prompt) {
    gui->showInputPrompt(prompt);
    while (!gui->shouldExit()) {
        gui->update();
        gui->display();
        std::string command = gui->getCommand();
        if (!command.empty() && command != "NULL") return command;
    }
    return "";
}

int BankruptcyManager::calculateSaleValue(const Property* property) const {
    if (property == nullptr || property->isMortgaged()) return 0;

    int total = property->getPurchasePrice();
    if (property->isStreet()) {
        auto* street = static_cast<const StreetProperty*>(property);
        total += street->sellBuildingValue();
    }
    return total;
}

int BankruptcyManager::calculateMortgageValue(const Property* property) const {
    if (property == nullptr || property->isMortgaged()) return 0;

    if (property->isStreet()) {
        auto* street = static_cast<const StreetProperty*>(property);
        if (street->getBuildingState() != BuildingState::NONE) return 0;
    }
    return property->getMortgageValue();
}

std::vector<BankruptcyManager::LiquidationOption>
BankruptcyManager::buildSellOptions(const Player& player) const {
    std::vector<LiquidationOption> options;
    for (Property* property : player.getOwnedProperties()) {
        int saleValue = calculateSaleValue(property);
        if (saleValue > 0) {
            options.emplace_back(LiquidationOption::Kind::SELL, property, saleValue);
        }
    }
    return options;
}

std::vector<BankruptcyManager::LiquidationOption>
BankruptcyManager::buildMortgageOptions(const Player& player) const {
    std::vector<LiquidationOption> options;
    for (Property* property : player.getOwnedProperties()) {
        int mortgageValue = calculateMortgageValue(property);
        if (mortgageValue > 0) {
            options.emplace_back(LiquidationOption::Kind::MORTGAGE, property, mortgageValue);
        }
    }
    return options;
}

std::vector<BankruptcyManager::LiquidationOption>
BankruptcyManager::buildEstimatePlan(const Player& player) const {
    std::vector<LiquidationOption> plan;
    for (Property* property : player.getOwnedProperties()) {
        auto* street = property->isStreet() ? static_cast<StreetProperty*>(property) : nullptr;
        if (street != nullptr && street->getBuildingState() != BuildingState::NONE) {
            int saleValue = calculateSaleValue(property);
            if (saleValue > 0) {
                plan.emplace_back(LiquidationOption::Kind::SELL, property, saleValue);
            }
            continue;
        }

        int mortgageValue = calculateMortgageValue(property);
        if (mortgageValue > 0) {
            plan.emplace_back(LiquidationOption::Kind::MORTGAGE, property, mortgageValue);
            continue;
        }

        int saleValue = calculateSaleValue(property);
        if (saleValue > 0) {
            plan.emplace_back(LiquidationOption::Kind::SELL, property, saleValue);
        }
    }
    return plan;
}

int BankruptcyManager::calculateMaxLiquidation(const Player& player) const {
    int total = player.getBalance();
    for (Property* property : player.getOwnedProperties()) {
        total += std::max(calculateSaleValue(property), calculateMortgageValue(property));
    }
    return total;
}

bool BankruptcyManager::canCoverDebt(const Player& player, int amount) const {
    return calculateMaxLiquidation(player) >= amount;
}

void BankruptcyManager::showLiquidationEstimate(const Player& debtor, int amount, Player* /*creditor*/,
                                                const std::string& obligationLabel) const {
    std::string label = obligationLabel.empty()
        ? "kewajiban " + Formatter::money(amount)
        : obligationLabel;

    gui->showMessage("Kamu tidak dapat membayar " + label + ".");
    gui->showMessage("");
    gui->showMessage("Uang kamu       : " + Formatter::money(debtor.getBalance()));
    gui->showMessage("Total kewajiban : " + Formatter::money(amount));

    int shortage = std::max(0, amount - debtor.getBalance());
    if (shortage > 0) {
        gui->showMessage("Kekurangan      : " + Formatter::money(shortage));
    }
    gui->showMessage("");

    auto plan = buildEstimatePlan(debtor);
    int potential = calculateMaxLiquidation(debtor) - debtor.getBalance();

    gui->showMessage("Estimasi dana dari likuidasi:");
    if (plan.empty()) {
        gui->showMessage("  Tidak ada aset yang dapat dilikuidasi.");
    } else if (potential + debtor.getBalance() < amount) {
        gui->showMessage("  Jual semua properti + bangunan -> " + Formatter::money(potential));
    } else {
        for (const auto& option : plan) {
            std::string action = option.kind == LiquidationOption::Kind::SELL ? "Jual " : "Gadai ";
            gui->showMessage("  " + action + option.property->getName() + " (" +
                             option.property->getCode() + ") [" +
                             propertyColorLabel(option.property) + "] -> " +
                             Formatter::money(option.value));
        }
        gui->showMessage("  Total potensi        -> " + Formatter::money(potential));
    }

    if (potential + debtor.getBalance() < amount) {
        gui->showMessage("Total aset + uang tunai          : " +
                         Formatter::money(debtor.getBalance() + potential));
        gui->showMessage("Tidak cukup untuk menutup kewajiban " +
                         Formatter::money(amount) + ".");
    } else {
        gui->showMessage("");
        gui->showMessage("Dana dari likuidasi cukup untuk menutup kewajiban.");
        gui->showMessage("Kamu harus melikuidasi aset untuk membayar.");
    }

}

int BankruptcyManager::sellPropertyToBank(Player& player, Property* property) {
    if (property == nullptr || property->isMortgaged()) return 0;

    int propertyValue = property->getPurchasePrice();
    int buildingValue = 0;
    if (property->isStreet()) {
        auto* street = static_cast<StreetProperty*>(property);
        buildingValue = street->sellBuildingValue();
        street->clearBuildings();
    }

    int total = propertyValue + buildingValue;
    player.removeProperty(property);
    property->clearOwner();
    property->setStatus(PropertyStatus::BANK);
    player.addMoney(total);

    if (logger != nullptr) {
        logger->log(game->getCurrentTurn(), player.getUsername(),
                    "JUAL_BANK",
                    property->getName() + " (" + property->getCode() + ") +" + std::to_string(total));
    }

    gui->showMessage(property->getName() + " terjual ke Bank.");
    gui->showMessage("Kamu menerima " +
                     Formatter::money(total) + ".");
    gui->showMessage("Uang kamu sekarang: " + Formatter::money(player.getBalance()));
    return total;
}

int BankruptcyManager::mortgageProperty(Player& player, Property* property) {
    int value = calculateMortgageValue(property);
    if (property == nullptr || value <= 0) return 0;

    property->setStatus(PropertyStatus::MORTGAGED);
    player.addMoney(value);

    if (logger != nullptr) {
        logger->log(game->getCurrentTurn(), player.getUsername(),
                    "GADAI",
                    property->getName() + " (" + property->getCode() + ") +" + std::to_string(value));
    }

    gui->showMessage(property->getName() + " berhasil digadaikan.");
    gui->showMessage("Kamu menerima " + Formatter::money(value) + " dari Bank.");
    gui->showMessage("Uang kamu sekarang: " + Formatter::money(player.getBalance()));
    return value;
}

bool BankruptcyManager::runLiquidationPanel(Player& player, int targetAmount) {
    while (!gui->shouldExit()) {
        if (player.getBalance() >= targetAmount) {
            return true;
        }

        auto sellOptions = buildSellOptions(player);
        auto mortgageOptions = buildMortgageOptions(player);
        if (sellOptions.empty() && mortgageOptions.empty()) {
            return false;
        }

        gui->showMessage("");
        gui->showMessage("Panel likuidasi:");
        gui->showMessage("Uang kamu saat ini: " + Formatter::money(player.getBalance()) +
                         "  |  Kewajiban: " + Formatter::money(targetAmount));

        int index = 1;
        std::vector<LiquidationOption> numbered;

        if (!sellOptions.empty()) {
            gui->showMessage("");
            gui->showMessage("Jual ke Bank:");
            for (const auto& option : sellOptions) {
                std::string detail = std::to_string(index) + ". " +
                                     option.property->getName() + " (" +
                                     option.property->getCode() + ") [" +
                                     propertyColorLabel(option.property) + "] Harga Jual: " +
                                     Formatter::money(option.value);
                if (option.property->isStreet()) {
                    auto* street = static_cast<StreetProperty*>(option.property);
                    int buildingValue = street->sellBuildingValue();
                    if (buildingValue > 0) {
                        detail += " (termasuk " + buildingLabel(street) + ": " +
                                  Formatter::money(buildingValue) + ")";
                    }
                }
                gui->showMessage(detail);
                numbered.push_back(option);
                ++index;
            }
        }

        if (!mortgageOptions.empty()) {
            gui->showMessage("");
            gui->showMessage("Gadaikan:");
            for (const auto& option : mortgageOptions) {
                gui->showMessage(std::to_string(index) + ". " +
                                 option.property->getName() + " (" +
                                 option.property->getCode() + ") [" +
                                 propertyColorLabel(option.property) + "] Nilai Gadai: " +
                                 Formatter::money(option.value));
                numbered.push_back(option);
                ++index;
            }
        }

        std::string input = waitForInput(gui, "Pilih aksi (0 jika sudah cukup):");
        int choice = -1;
        try {
            choice = std::stoi(input);
        } catch (...) {
            choice = -1;
        }

        if (choice == 0) {
            if (player.getBalance() >= targetAmount) return true;
            gui->showMessage("Dana kamu masih belum cukup untuk menutup kewajiban.");
            continue;
        }

        if (choice < 1 || choice > static_cast<int>(numbered.size())) {
        gui->showMessage("Nomor aksi tidak valid.");
            continue;
        }

        const auto& selected = numbered[choice - 1];
        if (selected.kind == LiquidationOption::Kind::SELL) {
            sellPropertyToBank(player, selected.property);
        } else {
            mortgageProperty(player, selected.property);
        }
    }

    return false;
}

void BankruptcyManager::transferAssetsToPlayer(Player& from, Player& to) {
    int remaining = from.getBalance();
    if (remaining > 0) {
        to.addMoney(remaining);
        from.deductMoney(remaining);
    }

    auto owned = from.getOwnedProperties();
    for (Property* property : owned) {
        from.removeProperty(property);
        property->setOwner(&to);
        to.addProperty(property);
    }
}

void BankruptcyManager::returnAssetsToBank(Player& player) {
    auto owned = player.getOwnedProperties();
    for (Property* property : owned) {
        player.removeProperty(property);
        property->clearOwner();
        property->setStatus(PropertyStatus::BANK);
        if (property->isStreet()) {
            auto* street = static_cast<StreetProperty*>(property);
            street->clearBuildings();
        }
    }

    int remaining = player.getBalance();
    if (remaining > 0) {
        player.deductMoney(remaining);
        gui->showMessage("Sisa uang " + Formatter::money(remaining) + " diserahkan ke Bank.");
    }

    gui->showMessage("Semua properti dikembalikan ke Bank.");
    gui->showMessage("Semua bangunan dihancurkan dan stok kembali ke Bank.");

    if (!owned.empty()) {
        gui->showMessage("");
        gui->showMessage("Properti akan dilelang satu per satu:");
        for (Property* property : owned) {
            gui->showMessage("  -> " + property->getName() + " (" +
                             property->getCode() + ")");
            if (auctionManager != nullptr) {
                auctionManager->runAuction(property, &player);
            }
        }
    }
}

void BankruptcyManager::declareBankruptcy(Player& debtor, Player* creditor) {
    gui->showMessage("");
    gui->showMessage(debtor.getUsername() + " dinyatakan bangkrut.");
    gui->showMessage("Kreditor: " + std::string(creditor ? creditor->getUsername() : "Bank"));
    debtor.setStatus(PlayerStatus::BANKRUPT);

    if (creditor != nullptr) {
        gui->showMessage("");
        gui->showMessage("Aset dialihkan ke " + creditor->getUsername() + ":");
        if (debtor.getBalance() > 0) {
            gui->showMessage("  - Uang tunai sisa  : " + Formatter::money(debtor.getBalance()));
        }
        for (Property* property : debtor.getOwnedProperties()) {
            gui->showMessage("  - " + property->getName() + " (" + property->getCode() + ") [" +
                             propertyColorLabel(property) + "] " +
                             ownershipStatusLabel(property));
        }
        transferAssetsToPlayer(debtor, *creditor);
        gui->showMessage(creditor->getUsername() + " menerima seluruh aset milik " +
                         debtor.getUsername() + ".");
    } else {
        returnAssetsToBank(debtor);
    }
    if (logger != nullptr) {
        logger->log(game->getCurrentTurn(), debtor.getUsername(),
                    "BANGKRUT",
                    std::string("Kreditor: ") + (creditor ? creditor->getUsername() : "BANK"));
    }

    gui->showMessage(debtor.getUsername() + " keluar dari permainan.");
    gui->showMessage("Permainan berlanjut dengan " +
                     std::to_string(game->getActivePlayerCount()) + " pemain tersisa.");
}

bool BankruptcyManager::handleInsufficientFunds(Player& debtor, int amount, Player* creditor,
                                                const std::string& obligationLabel) {
    if (amount <= 0) return true;

    if (debtor.canAfford(amount)) {
        debtor.deductMoney(amount);
        if (creditor != nullptr) creditor->addMoney(amount);
        return true;
    }

    showLiquidationEstimate(debtor, amount, creditor, obligationLabel);

    if (!canCoverDebt(debtor, amount)) {
        if (logger != nullptr) {
            logger->log(game->getCurrentTurn(), debtor.getUsername(),
                        "GAGAL_BAYAR",
                        (obligationLabel.empty() ? Formatter::money(amount) : obligationLabel) +
                        " -> bangkrut");
        }
        declareBankruptcy(debtor, creditor);
        return false;
    }

    if (!runLiquidationPanel(debtor, amount)) {
        if (logger != nullptr) {
            logger->log(game->getCurrentTurn(), debtor.getUsername(),
                        "GAGAL_BAYAR",
                        (obligationLabel.empty() ? Formatter::money(amount) : obligationLabel) +
                        " -> bangkrut");
        }
        declareBankruptcy(debtor, creditor);
        return false;
    }

    gui->showMessage("");
    gui->showMessage("Dana yang dibutuhkan sudah terpenuhi.");
    gui->showMessage("Membayar ke " +
                     std::string(creditor ? creditor->getUsername() : "Bank") + "...");

    int debtorBefore = debtor.getBalance();
    int creditorBefore = creditor ? creditor->getBalance() : 0;
    debtor.deductMoney(amount);
    if (creditor != nullptr) creditor->addMoney(amount);

    gui->showMessage("Uang kamu : " + Formatter::money(debtorBefore) + " -> " +
                     Formatter::money(debtor.getBalance()));
    if (creditor != nullptr) {
        gui->showMessage("Uang " + creditor->getUsername() + ": " +
                         Formatter::money(creditorBefore) + " -> " +
                         Formatter::money(creditor->getBalance()));
    }
    return true;
}
