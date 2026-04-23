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

std::string GameEngine::waitForInput(IGUI* gui, const std::string& prompt) {
    gui->showInputPrompt(prompt);
    while (!gui->shouldExit()) {
        gui->update();
        gui->display();
        std::string c = gui->getCommand();
        if (!c.empty() && c != "NULL") return c;
    }
    return "";
}

std::string GameEngine::normalizeInput(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return s;
}

bool GameEngine::askYesNo(IGUI* gui, const std::string& prompt) {
    while (!gui->shouldExit()) {
        std::string ans = normalizeInput(waitForInput(gui, prompt));
        if (ans == "Y" || ans == "YA" || ans == "YES") return true;
        if (ans == "N" || ans == "TIDAK" || ans == "NO") return false;
        gui->showMessage("Masukan tidak valid. Gunakan y/n.");
    }
    return false;
}

int GameEngine::askIncomeTaxChoice(IGUI* gui) {
    while (!gui->shouldExit()) {
        std::string ans = waitForInput(gui,
            "Pilih opsi pembayaran pajak: 1) Flat  2) 10% kekayaan");
        if (ans == "1" || ans == "2") return std::stoi(ans);
        gui->showMessage("Pilihan tidak valid. Masukkan 1 atau 2.");
    }
    return 1;
}

CommandResult GameEngine::resolveRoll(Player* player, bool manual, int d1, int d2, bool fromJailAttempt) {
    if (player == nullptr || turnManager == nullptr || dice == nullptr || gui == nullptr) {
        return CommandResult::INVALID;
    }

    if (!turnManager->canRoll(player)) {
        gui->showMessage("Tidak dapat melempar dadu sekarang.");
        return CommandResult::INVALID;
    }

    if (fromJailAttempt) {
        gui->showMessage("Mencoba keluar dari Penjara dengan melempar dadu.");
    }

    if (manual) {
        gui->showMessage("Dadu diatur secara manual.");
        dice->setManual(d1, d2);
    } else {
        gui->showMessage("Mengocok dadu...");
        dice->rollRandom();
    }

    int total = dice->getTotal();
    bool rolledDouble = dice->isDouble();
    gui->renderDice(dice->getDie1(), dice->getDie2());
    game->setLastDiceTotal(total);

    if (fromJailAttempt && player->isJailed()) {
        player->markRolled();
        turnManager->setPhase(TurnPhase::POST_ROLL);
        turnManager->markActed();

        if (!rolledDouble) {
            player->incrementJailAttempts();
            gui->showMessage("Belum mendapatkan double. Kamu tetap berada di Penjara dan tidak bergerak.");
            gui->showMessage("Percobaan keluar penjara: " +
                             std::to_string(player->getJailAttempts()) + "/3.");
            return CommandResult::END_TURN;
        }

        gui->showMessage("Double! Kamu bebas dari Penjara.");
        player->setStatus(PlayerStatus::ACTIVE);
        player->resetJailAttempts();
    } else {
        player->markRolled();
    }

    if (rolledDouble) {
        player->incrementConsecutiveDoubles();
        if (player->getConsecutiveDoubles() >= 3) {
            Board* board = game->getBoard();
            if (board != nullptr && board->getJailTile() != nullptr) {
                player->setPosition(board->getJailTile()->getIndex());
            }
            player->setStatus(PlayerStatus::JAILED);
            player->resetJailAttempts();
            gui->showMessage("Dadu double ketiga berturut-turut.");
            gui->showMessage("Bidak tidak digerakkan sesuai hasil dadu dan kamu langsung masuk Penjara.");
            turnManager->endTurn(player);
            return CommandResult::END_TURN;
        }
    } else {
        player->resetConsecutiveDoubles();
    }

    Tile* landed = turnManager->processMovement(player, total);
    turnManager->setPhase(TurnPhase::POST_ROLL);
    turnManager->markActed();

    std::string landedName = landed ? landed->getName() : "?";
    gui->renderMovement(player->getUsername(), total, landedName);

    if (landed) handleTileLanding(player, landed);

    if (player->getStatus() == PlayerStatus::JAILED) {
        gui->showMessage("Giliran berakhir karena kamu berada di Penjara.");
        turnManager->endTurn(player);
        return CommandResult::END_TURN;
    }

    if (player->getStatus() == PlayerStatus::BANKRUPT) {
        if (checkWinCondition()) return CommandResult::GAME_OVER;
        return CommandResult::CONTINUE;
    }

    if (player->hasPendingFestival()) {
        if (rolledDouble) {
            gui->showMessage("Lanjutkan dengan FESTIVAL <kode_properti> untuk menyelesaikan aksi petak ini.");
            gui->showMessage("Setelah itu, gunakan LEMPAR_DADU atau ATUR_DADU <x> <y> untuk lemparan bonus.");
        } else {
            gui->showMessage("Lanjutkan dengan FESTIVAL <kode_properti>, lalu gunakan command lain atau AKHIRI_GILIRAN.");
        }
    } else if (rolledDouble) {
        if (fromJailAttempt) {
            gui->showMessage("Karena berhasil keluar dengan double, kamu juga mendapat lemparan bonus.");
        } else {
            gui->showMessage("Dadu double. Kamu mendapat lemparan bonus.");
        }
        gui->showMessage("Gunakan LEMPAR_DADU atau ATUR_DADU <x> <y> untuk lemparan berikutnya.");
    } else {
        gui->showMessage("Giliran lempar dadu selesai. Gunakan command lain atau AKHIRI_GILIRAN.");
        gui->showMessage("Ketik HELP untuk melihat command yang tersedia.");
    }

    return CommandResult::CONTINUE;
}

CommandResult GameEngine::handleJailedPlayerTurn(Player* player) {
    if (player == nullptr || !player->isJailed()) {
        return CommandResult::CONTINUE;
    }

    const int fine = game->getJailFine();

    while (!gui->shouldExit()) {
        gui->showMessage(player->getUsername() + " sedang berada di Penjara.");

        if (player->getJailAttempts() >= 3) {
            gui->showMessage("Ini adalah giliran ke-4 di Penjara.");
            gui->showMessage("Kamu wajib membayar denda M" + std::to_string(fine) + " sebelum melempar dadu.");
            bool paid = executePayment(player, nullptr, fine);
            if (paid) {
                player->setStatus(PlayerStatus::ACTIVE);
                player->resetJailAttempts();
                gui->showMessage("Denda penjara dibayar. Kamu bebas dari Penjara.");
                gui->showMessage("Sekarang kamu bisa melempar dadu seperti biasa.");
                return CommandResult::CONTINUE;
            }
            if (player->getStatus() == PlayerStatus::BANKRUPT && checkWinCondition()) {
                return CommandResult::GAME_OVER;
            }
            return CommandResult::END_TURN;
        }

        SkillCard* jailCard = player->findJailFreeCard();
        gui->showMessage("Pilih cara keluar penjara:");
        gui->showMessage("1) Bayar denda M" + std::to_string(fine));
        gui->showMessage("2) Gunakan kartu Bebas dari Penjara" +
                         std::string(jailCard ? "" : " (tidak tersedia)"));
        gui->showMessage("3) Coba lempar dadu dan harus mendapatkan double");

        std::string choice = normalizeInput(waitForInput(gui, "Opsi penjara (1/2/3):"));

        if (choice == "1") {
            bool paid = executePayment(player, nullptr, fine);
            if (paid) {
                player->setStatus(PlayerStatus::ACTIVE);
                player->resetJailAttempts();
                gui->showMessage("Denda penjara dibayar. Kamu bebas dari Penjara.");
                gui->showMessage("Sekarang kamu bisa melempar dadu seperti biasa.");
                return CommandResult::CONTINUE;
            }
            if (player->getStatus() == PlayerStatus::BANKRUPT && checkWinCondition()) {
                return CommandResult::GAME_OVER;
            }
            return CommandResult::END_TURN;
        }

        if (choice == "2") {
            if (jailCard == nullptr) {
                gui->showMessage("Kamu tidak memiliki kartu Bebas dari Penjara.");
                continue;
            }
            player->removeCard(jailCard);
            if (game->getSkillDeck() != nullptr) {
                game->getSkillDeck()->discard(jailCard);
            }
            player->setStatus(PlayerStatus::ACTIVE);
            player->resetJailAttempts();
            gui->showMessage("Kartu Bebas dari Penjara digunakan. Kamu bebas dari Penjara.");
            gui->showMessage("Sekarang kamu bisa melempar dadu seperti biasa.");
            return CommandResult::CONTINUE;
        }

        if (choice == "3") {
            gui->showMessage("Gunakan LEMPAR_DADU atau ATUR_DADU <x> <y> untuk mencoba keluar dari Penjara.");
            gui->showMessage("Command informasi yang tetap tersedia: HELP, CETAK_PAPAN, CETAK_AKTA <kode>, CETAK_PROPERTI, CETAK_LOG [n], SIMPAN <file>, EXIT.");

            while (!gui->shouldExit()) {
                std::string raw = waitForInput(gui, "Perintah penjara:");
                if (raw.empty() || raw == "NULL") continue;

                std::istringstream iss(raw);
                std::vector<std::string> tokens;
                std::string tok;
                while (iss >> tok) tokens.push_back(tok);
                if (tokens.empty()) continue;

                std::string cmd = normalizeInput(tokens[0]);

                if (cmd == "HELP") {
                    gui->showMessage("Saat di Penjara, pilih salah satu:");
                    gui->showMessage("1) Bayar denda, 2) Gunakan kartu Bebas dari Penjara, 3) Coba lempar dadu.");
                    gui->showMessage("Untuk opsi lempar dadu, gunakan LEMPAR_DADU atau ATUR_DADU <x> <y>.");
                    continue;
                }

                if (cmd == "LEMPAR_DADU") {
                    return resolveRoll(player, false, 0, 0, true);
                }

                if (cmd == "ATUR_DADU") {
                    if (tokens.size() < 3) {
                        gui->showMessage("Format: ATUR_DADU <d1> <d2>");
                        continue;
                    }
                    int d1 = 0, d2 = 0;
                    try {
                        d1 = std::stoi(tokens[1]);
                        d2 = std::stoi(tokens[2]);
                    } catch (...) {
                        gui->showMessage("Nilai dadu tidak valid.");
                        continue;
                    }
                    if (d1 < 1 || d1 > 6 || d2 < 1 || d2 > 6) {
                        gui->showMessage("Nilai dadu harus berada pada rentang 1 sampai 6.");
                        continue;
                    }
                    return resolveRoll(player, true, d1, d2, true);
                }

                if (cmd == "CETAK_PAPAN" || cmd == "CETAK_AKTA" || cmd == "CETAK_DEED" || cmd == "CETAK_PROPERTI" ||
                    cmd == "CETAK_LOG" || cmd == "SIMPAN" || cmd == "EXIT") {
                    CommandResult res = commandProcessor->process(raw, player);
                    if (res == CommandResult::GAME_OVER || res == CommandResult::SAVED_MID_TURN) {
                        return res;
                    }
                    continue;
                }

                gui->showMessage("Saat mencoba keluar dari Penjara, gunakan LEMPAR_DADU atau ATUR_DADU <x> <y>.");
            }
        }

        gui->showMessage("Pilihan tidak valid. Masukkan 1, 2, atau 3.");
    }

    game->setGameOver(true);
    return CommandResult::GAME_OVER;
}

void GameEngine::handleChanceLanding(Player* player, ChanceTile* /*tile*/) {
    if (player == nullptr || game == nullptr || game->getChanceDeck() == nullptr ||
        game->getChanceDeck()->isEmpty()) {
        return;
    }

    ChanceCard* card = game->getChanceDeck()->draw();
    if (card == nullptr) return;

    gui->showMessage(player->getUsername() + " mengambil kartu Kesempatan: " + card->getDescription());

    Board* board = game->getBoard();
    switch (card->getType()) {
        case ChanceType::GO_TO_NEAREST_STATION: {
            if (board != nullptr) {
                RailroadTile* nearest = board->getNearestRailroad(player->getPosition());
                if (nearest != nullptr) {
                    player->setPosition(nearest->getIndex());
                    gui->showMessage(player->getUsername() + " pindah ke stasiun terdekat.");
                    gui->showMessage("Bidak mendarat di: " + nearest->getName() + ".");
                    handleTileLanding(player, nearest);
                }
            }
            break;
        }
        case ChanceType::MOVE_BACK_3: {
            if (board != nullptr) {
                const int boardSize = board->getSize() > 0 ? board->getSize() : 40;
                const int newPos = (player->getPosition() - 3 + boardSize) % boardSize;
                player->setPosition(newPos);
                Tile* target = board->getTile(newPos);
                gui->showMessage(player->getUsername() + " mundur 3 petak.");
                if (target != nullptr) {
                    gui->showMessage("Bidak mendarat di: " + target->getName() + ".");
                    handleTileLanding(player, target);
                }
            }
            break;
        }
        case ChanceType::GO_TO_JAIL: {
            player->setStatus(PlayerStatus::JAILED);
            player->resetJailAttempts();
            if (board != nullptr && board->getJailTile() != nullptr) {
                player->setPosition(board->getJailTile()->getIndex());
            }
            gui->showMessage(player->getUsername() + " masuk Penjara!");
            break;
        }
    }

    game->getChanceDeck()->discard(card);
}

void GameEngine::handleCommunityChestLanding(Player* player, CommunityChestTile* /*tile*/) {
    if (player == nullptr || game == nullptr || game->getCommunityDeck() == nullptr ||
        game->getCommunityDeck()->isEmpty()) {
        return;
    }

    CommunityChestCard* card = game->getCommunityDeck()->draw();
    if (card == nullptr) return;

    gui->showMessage(player->getUsername() + " mengambil kartu Dana Umum: " + card->getDescription());

    switch (card->getType()) {
        case CommunityType::BIRTHDAY: {
            for (Player* other : game->getActivePlayers()) {
                if (other == player) continue;
                const int amount = 100;
                if (executePayment(other, player, amount)) {
                    gui->showMessage(other->getUsername() + " memberi M" + std::to_string(amount) +
                                     " kepada " + player->getUsername() + " (ulang tahun).");
                }
            }
            break;
        }
        case CommunityType::DOCTOR_FEE: {
            const int fee = 700;
            if (executePayment(player, nullptr, fee)) {
                gui->showMessage(player->getUsername() + " membayar biaya dokter M" +
                                 std::to_string(fee) + ".");
            }
            break;
        }
        case CommunityType::CAMPAIGN_FEE: {
            const int fee = 200;
            for (Player* other : game->getActivePlayers()) {
                if (other == player) continue;
                if (!executePayment(player, other, fee)) break;
                gui->showMessage(player->getUsername() + " membayar M" + std::to_string(fee) +
                                 " ke " + other->getUsername() + " (biaya kampanye).");
            }
            break;
        }
    }

    game->getCommunityDeck()->discard(card);
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

    if (player->isJailed()) {
        CommandResult jailResult = handleJailedPlayerTurn(player);
        if (jailResult == CommandResult::END_TURN) {
            if (turnManager->getPhase() != TurnPhase::ENDED) {
                turnManager->endTurn(player);
            }
            return;
        }
        if (jailResult == CommandResult::GAME_OVER) {
            game->setGameOver(true);
            return;
        }
        if (jailResult == CommandResult::SAVED_MID_TURN) {
            return;
        }
        gui->renderPlayer(*player);
    }

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

    if (auto* chanceTile = dynamic_cast<ChanceTile*>(tile)) {
        handleChanceLanding(player, chanceTile);
        return;
    }

    if (auto* communityTile = dynamic_cast<CommunityChestTile*>(tile)) {
        handleCommunityChestLanding(player, communityTile);
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
