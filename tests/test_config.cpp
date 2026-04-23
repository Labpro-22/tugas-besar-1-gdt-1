#include <iostream>
#include <exception>

#include "utils/data/ConfigLoader.hpp"
#include "models/BoardAndTiles/Board.hpp"
#include "models/BoardAndTiles/Tile.hpp"

static void printSeparator(const std::string& title) {
    std::cout << "\n--- " << title << " ---\n";
}

int main() {
    try {
        ConfigLoader loader("data/default");
        GameConfig config = loader.loadGameConfig();

        printSeparator("MiscConfig");
        std::cout << "maxTurn        = " << config.getMisc().getMaxTurn() << "\n";
        std::cout << "initialBalance = " << config.getMisc().getInitialBalance() << "\n";

        printSeparator("SpecialConfig");
        std::cout << "goSalary = " << config.getSpecial().getGoSalary() << "\n";
        std::cout << "jailFine = " << config.getSpecial().getJailFine() << "\n";

        printSeparator("TaxConfig");
        std::cout << "pphFlat    = " << config.getTax().getPphFlat() << "\n";
        std::cout << "pphPercent = " << config.getTax().getPphPercent() << "\n";
        std::cout << "pbmFlat    = " << config.getTax().getPbmFlat() << "\n";

        printSeparator("RailroadRents");
        for (const auto& p : config.getRailroadRents()) {
            std::cout << "count=" << p.first << " rent=" << p.second << "\n";
        }

        printSeparator("UtilityMultipliers");
        for (const auto& p : config.getUtilityMultipliers()) {
            std::cout << "count=" << p.first << " mult=" << p.second << "\n";
        }

        printSeparator("Properties (non-null)");
        int propCount = 0;
        for (Property* prop : config.getProperties()) {
            if (prop != nullptr) {
                std::cout << "[" << prop->getCode() << "] " << prop->getName()
                          << " price=" << prop->getPurchasePrice()
                          << " mortgage=" << prop->getMortgageValue() << "\n";
                ++propCount;
            }
        }
        std::cout << "Total properti: " << propCount << "\n";

        printSeparator("Board");
        Board* board = loader.buildBoard(config.getProperties(), config);
        std::cout << "Jumlah tile di board: " << board->getSize() << "\n";
        delete board;

        printSeparator("Decks");
        auto decks = loader.buildDecks();
        std::cout << "ChanceDeck size        = " << std::get<0>(decks)->size() << "\n";
        std::cout << "CommunityChestDeck size = " << std::get<1>(decks)->size() << "\n";
        std::cout << "SkillDeck size         = " << std::get<2>(decks)->size() << "\n";
        delete std::get<0>(decks);
        delete std::get<1>(decks);
        delete std::get<2>(decks);

        std::cout << "\n[OK] Config loading selesai.\n";
    } catch (const std::exception& e) {
        std::cerr << "\n[ERROR] " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
