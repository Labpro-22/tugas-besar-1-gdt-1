#include "core/GameEngine.hpp"
#include "core/CommandProcessor.hpp"
#include "core/AuctionManager.hpp"
#include "core/BankruptcyManager.hpp"
#include "core/SaveLoadManager.hpp"

#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <numeric>

#include "models/Player/Player.hpp"
#include "models/BoardAndTiles/Board.hpp"
#include "models/BoardAndTiles/Tile.hpp"
#include "models/BoardAndTiles/PropertyTile.hpp"
#include "models/BoardAndTiles/SpecialTile.hpp"
#include "models/BoardAndTiles/ActionTile.hpp"
#include "models/BoardAndTiles/PropertyTile/StreetTile.hpp"
#include "models/BoardAndTiles/PropertyTile/RailroadTile.hpp"
#include "models/BoardAndTiles/PropertyTile/UtilityTile.hpp"
#include "models/BoardAndTiles/ActionTile/ChanceTile.hpp"
#include "models/BoardAndTiles/ActionTile/CommunityChestTile.hpp"
#include "models/BoardAndTiles/ActionTile/FestivalTile.hpp"
#include "models/BoardAndTiles/ActionTile/TaxTile.hpp"
#include "models/BoardAndTiles/ActionTile/TaxTile/IncomingTaxTile.hpp"
#include "models/BoardAndTiles/ActionTile/TaxTile/LuxuryTaxTile.hpp"
#include "models/BoardAndTiles/SpecialTile/GoToJailTile.hpp"
#include "models/BoardAndTiles/SpecialTile/JailTile.hpp"
#include "models/BoardAndTiles/SpecialTile/GoTile.hpp"
#include "models/BoardAndTiles/SpecialTile/FreeParkingTile.hpp"

GameEngine::GameEngine(IGUI* gui)
    : game(nullptr),
      logger(new TransactionLogger()),
      gui(gui),
      dice(new DiceManager()),
      turnManager(nullptr),
      commandProcessor(nullptr),
      auctionManager(nullptr),
      bankruptcyManager(nullptr),
      saveLoadManager(nullptr) {}

GameEngine::~GameEngine() {
    delete commandProcessor;
    delete auctionManager;
    delete bankruptcyManager;
    delete saveLoadManager;
    delete turnManager;
    delete dice;
    delete logger;
    delete game;
}

static std::string waitForInput(IGUI* gui, const std::string& prompt) {
    gui->showInputPrompt(prompt);
    while (!gui->shouldExit()) {
        gui->update();
        gui->display();
        std::string c = gui->getCommand();
        if (!c.empty() && c != "NULL") return c;
    }
    return "";
}

static std::string normalizeInput(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return s;
}

static bool askYesNo(IGUI* gui, const std::string& prompt) {
    while (!gui->shouldExit()) {
        std::string ans = normalizeInput(waitForInput(gui, prompt));
        if (ans == "Y" || ans == "YA" || ans == "YES") return true;
        if (ans == "N" || ans == "TIDAK" || ans == "NO") return false;
        gui->showMessage("Masukan tidak valid. Gunakan y/n.");
    }
    return false;
}

static int askIncomeTaxChoice(IGUI* gui) {
    while (!gui->shouldExit()) {
        std::string ans = waitForInput(gui,
            "Pilih opsi pembayaran pajak: 1) Flat  2) 10% kekayaan");
        if (ans == "1" || ans == "2") return std::stoi(ans);
        gui->showMessage("Pilihan tidak valid. Masukkan 1 atau 2.");
    }
    return 1;
}

void GameEngine::run() {
    if (gui == nullptr) {
        return;
    }

    gui->loadMainMenu();

    // Loop menu utama — menunggu user memilih NEW_GAME atau LOAD_GAME
    while (!gui->shouldExit()) {
        gui->update();
        gui->display();

        std::string command = gui->getCommand();
        if (command == "NULL" || command.empty()) {
            continue;
        }

        if (command == "NEW_GAME") {
            initNewGame();
            gameLoop();
            break;
        } else if (command.rfind("LOAD_GAME", 0) == 0) {
            initLoadGame();
            gameLoop();
            break;
        } else if (command == "EXIT") {
            break;
        }
    }
}

void GameEngine::initNewGame() {
    this->game = new Game();

    ConfigLoader loader("data/default");
    GameConfig config = loader.loadGameConfig();

    game->setConfigValues(
        config.getMisc().getMaxTurn(),
        config.getMisc().getInitialBalance(),
        config.getSpecial().getGoSalary(),
        config.getSpecial().getJailFine(),
        config.getTax().getPphFlat(),
        config.getTax().getPphPercent(),
        config.getTax().getPbmFlat(),
        config.getRailroadRents(),
        config.getUtilityMultipliers()
    );

    game->setBoard(loader.buildBoard(config.getProperties(), config));

    auto decks = loader.buildDecks();
    game->setDecks(std::get<0>(decks), std::get<1>(decks), std::get<2>(decks));

    turnManager = new TurnManager(game, dice, gui);
    commandProcessor = new CommandProcessor(this, game, turnManager, dice, gui);
    auctionManager = new AuctionManager(game, logger, gui);
    bankruptcyManager = new BankruptcyManager(game, logger, gui, auctionManager);
    saveLoadManager = new SaveLoadManager(game, logger, gui);

    int numPlayers = 0;
    while (numPlayers < 2 || numPlayers > 4) {
        std::string s = waitForInput(gui, "Jumlah pemain (2-4):");
        try { numPlayers = std::stoi(s); } catch (...) { numPlayers = 0; }
        if (numPlayers < 2 || numPlayers > 4) {
            gui->showMessage("Jumlah pemain harus 2-4.");
        }
    }

    int initBalance = config.getMisc().getInitialBalance();
    for (int i = 0; i < numPlayers; ++i) {
        std::string uname = waitForInput(gui,
            "Username pemain ke-" + std::to_string(i + 1) + ":");
        game->addPlayer(new Player(uname, initBalance));
    }

    std::vector<int> order(numPlayers);
    std::iota(order.begin(), order.end(), 0);
    unsigned seed = static_cast<unsigned>(
        std::chrono::system_clock::now().time_since_epoch().count());
    std::shuffle(order.begin(), order.end(), std::default_random_engine(seed));
    game->setTurnOrder(order);
    game->setCurrentTurnIndex(0);
    game->setCurrentTurn(1);

    // Set all players to start on GO (tile index 1)
    Board* b = game->getBoard();
    int goIndex = (b && b->getGoTile()) ? b->getGoTile()->getIndex() : 1;
    for (Player* p : game->getPlayers()) {
        p->setPosition(goIndex);
    }

    gui->loadGameView();
}

void GameEngine::initLoadGame() {
    this->game = new Game();

    ConfigLoader loader("data/default");
    GameConfig config = loader.loadGameConfig();

    game->setConfigValues(
        config.getMisc().getMaxTurn(),
        config.getMisc().getInitialBalance(),
        config.getSpecial().getGoSalary(),
        config.getSpecial().getJailFine(),
        config.getTax().getPphFlat(),
        config.getTax().getPphPercent(),
        config.getTax().getPbmFlat(),
        config.getRailroadRents(),
        config.getUtilityMultipliers()
    );
    game->setBoard(loader.buildBoard(config.getProperties(), config));
    auto decks = loader.buildDecks();
    game->setDecks(std::get<0>(decks), std::get<1>(decks), std::get<2>(decks));

    turnManager = new TurnManager(game, dice, gui);
    commandProcessor = new CommandProcessor(this, game, turnManager, dice, gui);
    auctionManager = new AuctionManager(game, logger, gui);
    bankruptcyManager = new BankruptcyManager(game, logger, gui, auctionManager);
    saveLoadManager = new SaveLoadManager(game, logger, gui);

    std::string filepath = waitForInput(gui, "Nama file save:");
    if (!saveLoadManager->load(filepath)) {
        gui->showMessage("Load gagal, memulai game baru.");
        return;
    }
    gui->showMessage("Game dimuat dari " + filepath);
    gui->loadGameView();
}

void GameEngine::gameLoop() {
    if (game == nullptr || turnManager == nullptr) return;

    while (!game->isGameOver() && !game->isMaxTurnReached()) {
        Player* current = game->getCurrentPlayer();
        if (current == nullptr) break;

        processPlayerTurn(current);

        if (checkWinCondition()) {
            game->setGameOver(true);
            break;
        }

        game->advanceTurnOrder();
        if (game->getCurrentTurnIndex() == 0) {
            game->incrementTurn();
        }
    }
    endGame();
}

void GameEngine::processPlayerTurn(Player* player) {
    turnManager->startTurn(player);
    gui->renderPlayer(*player);

    while (!gui->shouldExit()) {
        gui->update();
        gui->display();

        std::string cmd = gui->getCommand();
        if (cmd.empty() || cmd == "NULL") continue;

        CommandResult res = commandProcessor->process(cmd, player);
        if (res == CommandResult::END_TURN)       break;
        if (res == CommandResult::GAME_OVER)      { game->setGameOver(true); return; }
        if (res == CommandResult::SAVED_MID_TURN) return;
    }

    if (turnManager->getPhase() != TurnPhase::ENDED) {
        turnManager->endTurn(player);
    }
}

void GameEngine::handleTileLanding(Player* player, Tile* tile) {   
    if (player == nullptr || tile == nullptr || game == nullptr) {
        return;
    }

    if (auto* pt = dynamic_cast<PropertyTile*>(tile)) {
        Property* prop = pt->getProperty();
        if (prop == nullptr) return;

        if (prop->getStatus() == PropertyStatus::BANK) {
            if (prop->getType() == PropertyType::STREET) {
                gui->showMessage("Kamu mendarat di " + prop->getName() +
                                 " (" + prop->getCode() + ")!");
                gui->renderProperty(*prop);

                int price = prop->getPurchasePrice();
                if (player->canAfford(price) &&
                    askYesNo(gui, "Beli properti ini seharga M" + std::to_string(price) + "? (y/n)")) {
                    player->deductMoney(price);
                    prop->setOwner(player);
                    prop->setStatus(PropertyStatus::OWNED);
                    player->addProperty(prop);
                    gui->showMessage(prop->getName() + " kini menjadi milikmu. Uang tersisa: M"
                                     + std::to_string(player->getBalance()));
                    if (logger) {
                        logger->log(game->getCurrentTurn(), player->getUsername(),
                                    "BELI", prop->getCode() + " M" + std::to_string(price));
                    }
                } else {
                    gui->showMessage("Properti ini akan masuk ke sistem lelang.");
                    if (auctionManager != nullptr) {
                        auctionManager->runAuction(prop, player);
                    }
                }
                return;
            }

            prop->setOwner(player);
            prop->setStatus(PropertyStatus::OWNED);
            player->addProperty(prop);
            gui->showMessage(prop->getName() + " kini menjadi milikmu!");
            if (logger) {
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "BELI_OTOMATIS", prop->getCode());
            }
            return;
        }

        Player* owner = prop->getOwner();
        if (owner == player) {
            gui->showMessage("Kamu mendarat di propertimu sendiri (" + prop->getName() + ").");
            return;
        }
        if (prop->isMortgaged()) {
            gui->showMessage("Kamu mendarat di " + prop->getName() +
                             ", tetapi properti ini sedang digadaikan. Tidak ada sewa.");
            return;
        }

        int rent = prop->calculateRent(prop->getType() == PropertyType::UTILITY
                                       ? game->getLastDiceTotal()
                                       : 0);
        gui->showMessage("Kamu mendarat di " + prop->getName() + " (" + prop->getCode() +
                         "), milik " + owner->getUsername() + ".");
        gui->showMessage("Sewa: M" + std::to_string(rent));
        int payerBefore = player->getBalance();
        int ownerBefore = owner->getBalance();
        bool paid = executePayment(player, owner, rent);
        if (paid) {
            gui->showMessage("Uang kamu: M" + std::to_string(payerBefore) + " -> M"
                             + std::to_string(player->getBalance()));
            gui->showMessage("Uang " + owner->getUsername() + ": M" + std::to_string(ownerBefore)
                             + " -> M" + std::to_string(owner->getBalance()));
            if (logger) {
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "SEWA", prop->getCode() + " M" + std::to_string(rent));
            }
        }
        return;
    }

    if (auto* incomeTax = dynamic_cast<IncomeTaxTile*>(tile)) {
        gui->showMessage("Kamu mendarat di Pajak Penghasilan (PPH)!");
        int choice = askIncomeTaxChoice(gui);
        int amount = 0;
        if (choice == 1) {
            amount = incomeTax->getFlatAmount();
            gui->showMessage("Memilih bayar flat M" + std::to_string(amount) + ".");
        } else {
            int wealth = player->calculateTotalWealth();
            amount = (wealth * incomeTax->getTaxPercentage()) / 100;
            gui->showMessage("Total kekayaan kamu: M" + std::to_string(wealth));
            gui->showMessage("Pajak 10%: M" + std::to_string(amount));
        }
        int before = player->getBalance();
        bool paid = executePayment(player, nullptr, amount);
        if (paid) {
            gui->showMessage("Uang kamu: M" + std::to_string(before) + " -> M"
                             + std::to_string(player->getBalance()));
            if (logger) {
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "PAJAK", "PPH M" + std::to_string(amount));
            }
        }
        return;
    }

    if (auto* luxuryTax = dynamic_cast<LuxuryTaxTile*>(tile)) {
        int amount = luxuryTax->getFlatAmount();
        gui->showMessage("Kamu mendarat di Pajak Barang Mewah (PBM)!");
        gui->showMessage("Pajak sebesar M" + std::to_string(amount) + ".");
        int before = player->getBalance();
        bool paid = executePayment(player, nullptr, amount);
        if (paid) {
            gui->showMessage("Uang kamu: M" + std::to_string(before) + " -> M"
                             + std::to_string(player->getBalance()));
            if (logger) {
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "PAJAK", "PBM M" + std::to_string(amount));
            }
        }
        return;
    }

    if (dynamic_cast<FestivalTile*>(tile) != nullptr) {
        if (player->getOwnedProperties().empty()) {
            gui->showMessage(player->getUsername() +
                             " mendarat di Festival, tapi tidak memiliki properti. Festival hangus.");
            return;
        }
        player->setPendingFestival(true);
        gui->showMessage("Kamu mendarat di Festival!");
        gui->showMessage("Gunakan FESTIVAL <kode_properti> untuk memilih properti yang sewanya digandakan.");
        return;
    }

    if (auto* goTile = dynamic_cast<GoTile*>(tile)) {
        gui->showMessage(player->getUsername() + " mendarat tepat di petak GO dan menerima M"
                         + std::to_string(goTile->getSalary()) + ".");
        return;
    }

    if (dynamic_cast<GoToJailTile*>(tile) != nullptr) {
        player->setStatus(PlayerStatus::JAILED);
        if (game->getBoard() && game->getBoard()->getJailTile()) {
            player->setPosition(game->getBoard()->getJailTile()->getIndex());
        }
        gui->showMessage(player->getUsername() + " harus pergi ke Penjara.");
        return;
    }

    if (dynamic_cast<JailTile*>(tile) != nullptr) {
        if (player->isJailed()) {
            gui->showMessage(player->getUsername() + " sedang berada di Penjara.");
        } else {
            gui->showMessage(player->getUsername() + " hanya mampir ke Penjara.");
        }
        return;
    }

    if (dynamic_cast<FreeParkingTile*>(tile) != nullptr) {
        gui->showMessage(player->getUsername() + " mendarat di Bebas Parkir.");
        return;
    }

    tile->onLanded(*player, *game);
}

bool GameEngine::executePayment(Player* from, Player* to, int amount) {
    if (from == nullptr || amount <= 0) return false;
    if (bankruptcyManager != nullptr) {
        return bankruptcyManager->handleInsufficientFunds(*from, amount, to);
    }
    if (!from->canAfford(amount)) return false;
    from->deductMoney(amount);
    if (to != nullptr) to->addMoney(amount);
    return true;
}

bool GameEngine::checkWinCondition() {
    if (game == nullptr) return false;
    if (game->getActivePlayerCount() <= 1) return true;
    if (game->isMaxTurnReached()) return true;
    return false;
}

void GameEngine::endGame() {
    if (game == nullptr || gui == nullptr) return;

    Player* winner = nullptr;
    int bestWealth = -1;
    for (Player* p : game->getActivePlayers()) {
        int w = p->calculateTotalWealth();
        if (w > bestWealth) { bestWealth = w; winner = p; }
    }

    gui->loadFinishMenu();
    if (winner != nullptr) gui->renderWinner(*winner);
}
