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
#include <filesystem>
#include <sstream>

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

namespace {
std::string formatMoney(int amount) {
    return "M" + std::to_string(amount);
}

std::string tileLogLabel(const Tile* tile) {
    if (tile == nullptr) return "?";
    if (tile->getCode().empty()) return tile->getName();
    return tile->getName() + " (" + tile->getCode() + ")";
}

std::string propertyLogLabel(const Property* property) {
    if (property == nullptr) return "?";
    return property->getName() + " (" + property->getCode() + ")";
}

std::string buildingStateLabel(const StreetProperty* property) {
    if (property == nullptr) return "tanah kosong";

    switch (property->getBuildingState()) {
        case BuildingState::NONE:    return "tanah kosong";
        case BuildingState::HOUSE_1: return "1 rumah";
        case BuildingState::HOUSE_2: return "2 rumah";
        case BuildingState::HOUSE_3: return "3 rumah";
        case BuildingState::HOUSE_4: return "4 rumah";
        case BuildingState::HOTEL:   return "Hotel";
        default:                     return "tanah kosong";
    }
}

std::string rentConditionLabel(const Property* property) {
    if (property == nullptr) return "";

    std::string label;
    if (auto* street = dynamic_cast<const StreetProperty*>(property)) {
        label = buildingStateLabel(street);
    } else if (property->getType() == PropertyType::RAILROAD) {
        label = "Railroad";
    } else if (property->getType() == PropertyType::UTILITY) {
        label = "Utility";
    }

    if (property->getFestivalDuration() > 0) {
        if (!label.empty()) label += " | ";
        label += "Festival aktif x" + std::to_string(property->getFestivalMultiplier()) +
                 " (" + std::to_string(property->getFestivalDuration()) + " giliran)";
    }
    return label;
}
}

GameEngine::GameEngine(IGUI* gui)
    : game(nullptr),
      logger(new TransactionLogger()),
      gui(gui),
      dice(new DiceManager()),
      turnManager(nullptr),
      commandProcessor(nullptr),
      auctionManager(nullptr),
      bankruptcyManager(nullptr),
      saveLoadManager(nullptr),
      resumeLoadedTurn(false),
      pendingLoadRequested(false),
      skipAdvanceAfterLoad(false),
      pendingLoadPath() {}

GameEngine::~GameEngine() {
    resetRuntimeState();
    delete dice;
    delete logger;
}

void GameEngine::resetRuntimeState() {
    delete commandProcessor;
    commandProcessor = nullptr;
    delete auctionManager;
    auctionManager = nullptr;
    delete bankruptcyManager;
    bankruptcyManager = nullptr;
    delete saveLoadManager;
    saveLoadManager = nullptr;
    delete turnManager;
    turnManager = nullptr;
    delete game;
    game = nullptr;
}

void GameEngine::requestLoad(const std::string& filepath) {
    pendingLoadRequested = true;
    pendingLoadPath = filepath;
}

bool GameEngine::performPendingLoad() {
    if (!pendingLoadRequested) return true;
    const std::string filepath = pendingLoadPath;
    pendingLoadRequested = false;
    pendingLoadPath.clear();
    const bool ok = loadFromPath(filepath);
    if (ok) {
        skipAdvanceAfterLoad = true;
    }
    return ok;
}

bool GameEngine::loadFromPath(const std::string& filepath) {
    std::string configDir = "data/default";
    {
        namespace fs = std::filesystem;
        fs::path direct(filepath);
        fs::path saveDir = direct.has_parent_path() ? direct : (fs::path("data") / direct);
        if (fs::exists(saveDir) && fs::is_directory(saveDir)) {
            configDir = saveDir.string();
        }
    }

    ConfigLoader loader(configDir);
    GameConfig config = loader.loadGameConfig();

    Game* loadedGame = new Game();
    loadedGame->setConfigValues(
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
    loadedGame->setBoard(loader.buildBoard(config.getProperties(), config));
    auto decks = loader.buildDecks();
    loadedGame->setDecks(std::get<0>(decks), std::get<1>(decks), std::get<2>(decks));

    TurnManager* loadedTurnManager = new TurnManager(loadedGame, dice, gui);
    TransactionLogger loadedLogger;
    SaveLoadManager* loaderManager =
        new SaveLoadManager(loadedGame, &loadedLogger, loadedTurnManager, gui, configDir);

    if (!loaderManager->load(filepath)) {
        gui->showMessage("Gagal memuat permainan.");
        delete loaderManager;
        delete loadedTurnManager;
        delete loadedGame;
        return false;
    }

    resetRuntimeState();
    game = loadedGame;
    turnManager = loadedTurnManager;
    logger->loadEntries(loadedLogger.getFullLog());
    auctionManager = new AuctionManager(game, logger, gui);
    bankruptcyManager = new BankruptcyManager(game, logger, gui, auctionManager);
    commandProcessor = new CommandProcessor(this, game, turnManager, dice, gui);
    saveLoadManager = new SaveLoadManager(game, logger, turnManager, gui, configDir);
    delete loaderManager;

    if (logger != nullptr) {
        logger->log(game->getCurrentTurn(), "SISTEM",
                    "MUAT", "Game dimuat dari " + filepath);
    }
    gui->showMessage("Permainan berhasil dimuat dari " + filepath + ".");
    gui->loadGameView();
    resumeLoadedTurn = true;
    return true;
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
        std::string ans = waitForInput(gui, "Pilihan (1/2):");
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
        gui->showMessage("Kamu belum bisa melempar dadu sekarang.");
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
            if (logger != nullptr) {
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "DADU",
                            "Lempar: " + std::to_string(dice->getDie1()) + "+" +
                            std::to_string(dice->getDie2()) + "=" + std::to_string(total) +
                            " -> gagal keluar dari Penjara (" +
                            std::to_string(player->getJailAttempts()) + "/3)");
            }
            gui->showMessage("Kamu belum mendapatkan double.");
            gui->showMessage("Kamu tetap berada di Penjara dan tidak bergerak.");
            gui->showMessage("Percobaan keluar dari Penjara: " +
                             std::to_string(player->getJailAttempts()) + "/3.");
            return CommandResult::END_TURN;
        }

        if (logger != nullptr) {
            logger->log(game->getCurrentTurn(), player->getUsername(),
                        "PENJARA", "Bebas dari Penjara dengan double");
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
            if (logger != nullptr) {
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "DADU",
                            "Lempar: " + std::to_string(dice->getDie1()) + "+" +
                            std::to_string(dice->getDie2()) + "=" + std::to_string(total) +
                            " (double ketiga) -> masuk Penjara");
            }
            gui->showMessage("Kamu mendapatkan double tiga kali berturut-turut.");
            gui->showMessage("Bidak tidak digerakkan dan kamu langsung masuk Penjara.");
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
    if (logger != nullptr) {
        std::string detail = "Lempar: " + std::to_string(dice->getDie1()) + "+" +
                             std::to_string(dice->getDie2()) + "=" + std::to_string(total);
        if (rolledDouble) {
            detail += " (double)";
        }
        detail += " -> mendarat di " + tileLogLabel(landed);
        logger->log(game->getCurrentTurn(), player->getUsername(), "DADU", detail);
    }

    if (landed) handleTileLanding(player, landed);

    if (player->getStatus() == PlayerStatus::JAILED) {
        gui->showMessage("Giliranmu berakhir karena kamu berada di Penjara.");
        turnManager->endTurn(player);
        return CommandResult::END_TURN;
    }

    if (player->getStatus() == PlayerStatus::BANKRUPT) {
        if (checkWinCondition()) return CommandResult::GAME_OVER;
        return CommandResult::CONTINUE;
    }

    if (player->hasPendingFestival()) {
        if (rolledDouble) {
            gui->showMessage("Selesaikan dulu efek Festival dengan FESTIVAL <kode_properti>.");
            gui->showMessage("Setelah itu, gunakan LEMPAR_DADU atau ATUR_DADU <x> <y> untuk lemparan bonus.");
        } else {
            gui->showMessage("Selesaikan dulu efek Festival dengan FESTIVAL <kode_properti>.");
            gui->showMessage("Setelah itu, gunakan perintah lain atau AKHIRI_GILIRAN.");
        }
    } else if (rolledDouble) {
        if (logger != nullptr) {
            std::string detail;
            if (fromJailAttempt) {
                detail = "Mendapat lemparan bonus setelah bebas dari Penjara";
            } else {
                detail = "Giliran tambahan ke-" +
                         std::to_string(player->getConsecutiveDoubles());
            }
            logger->log(game->getCurrentTurn(), player->getUsername(), "DOUBLE", detail);
        }
        if (fromJailAttempt) {
            gui->showMessage("Karena berhasil keluar dengan double, kamu juga mendapat lemparan bonus.");
        } else {
            gui->showMessage("Dadu menunjukkan double. Kamu mendapat lemparan bonus.");
        }
        gui->showMessage("Gunakan LEMPAR_DADU atau ATUR_DADU <x> <y> untuk lemparan berikutnya.");
    } else {
        gui->showMessage("Aksi lempar dadu selesai.");
        gui->showMessage("Gunakan perintah lain atau AKHIRI_GILIRAN.");
        gui->showMessage("Ketik HELP untuk melihat daftar perintah yang tersedia.");
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
            gui->showMessage("Ini adalah giliran ke-4 kamu di Penjara.");
            gui->showMessage("Kamu wajib membayar denda M" + std::to_string(fine) + " sebelum melempar dadu.");
            bool paid = executePayment(player, nullptr, fine,
                                       "denda penjara " + formatMoney(fine));
            if (paid) {
                player->setStatus(PlayerStatus::ACTIVE);
                player->resetJailAttempts();
                if (logger != nullptr) {
                    logger->log(game->getCurrentTurn(), player->getUsername(),
                                "PENJARA",
                                "Bayar denda " + formatMoney(fine) + " dan bebas dari Penjara");
                }
                gui->showMessage("Denda penjara sudah dibayar. Kamu bebas dari Penjara.");
                gui->showMessage("Sekarang kamu dapat melempar dadu seperti biasa.");
                return CommandResult::CONTINUE;
            }
            if (player->getStatus() == PlayerStatus::BANKRUPT && checkWinCondition()) {
                return CommandResult::GAME_OVER;
            }
            return CommandResult::END_TURN;
        }

        SkillCard* jailCard = player->findJailFreeCard();
        gui->showMessage("Pilih cara untuk keluar dari Penjara:");
        gui->showMessage("1) Bayar denda M" + std::to_string(fine));
        gui->showMessage("2) Gunakan kartu Bebas dari Penjara" +
                         std::string(jailCard ? "" : " (tidak tersedia)"));
        gui->showMessage("3) Coba lempar dadu dan harus mendapatkan double");

        std::string choice = normalizeInput(waitForInput(gui, "Opsi penjara (1/2/3):"));

        if (choice == "1") {
            bool paid = executePayment(player, nullptr, fine,
                                       "denda penjara " + formatMoney(fine));
            if (paid) {
                player->setStatus(PlayerStatus::ACTIVE);
                player->resetJailAttempts();
                gui->showMessage("Denda penjara sudah dibayar. Kamu bebas dari Penjara.");
                gui->showMessage("Sekarang kamu dapat melempar dadu seperti biasa.");
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
            if (logger != nullptr) {
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "KARTU",
                            "Pakai " + jailCard->getCardName() + " -> bebas dari Penjara");
            }
            gui->showMessage("Kartu Bebas dari Penjara digunakan. Kamu bebas dari Penjara.");
            gui->showMessage("Sekarang kamu dapat melempar dadu seperti biasa.");
            return CommandResult::CONTINUE;
        }

        if (choice == "3") {
            gui->showMessage("Gunakan LEMPAR_DADU atau ATUR_DADU <x> <y> untuk mencoba keluar dari Penjara.");
            gui->showMessage("Perintah informasi yang tetap tersedia: HELP, CETAK_PAPAN, CETAK_AKTA <kode>, CETAK_PROPERTI, CETAK_LOG [n], SIMPAN <file>, EXIT.");

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
                    gui->showMessage("Saat berada di Penjara, pilih salah satu cara keluar:");
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
                    cmd == "CETAK_LOG" || cmd == "SIMPAN" || cmd == "MUAT" || cmd == "EXIT") {
                    CommandResult res = commandProcessor->process(raw, player);
                    if (res == CommandResult::GAME_OVER || res == CommandResult::SAVED_MID_TURN || res == CommandResult::LOADED_GAME) {
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
                    if (logger != nullptr) {
                        logger->log(game->getCurrentTurn(), player->getUsername(),
                                    "KARTU",
                                    "Kesempatan: " + card->getDescription() +
                                    " -> " + tileLogLabel(nearest));
                    }
                    gui->showMessage(player->getUsername() + " berpindah ke stasiun terdekat.");
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
                    if (logger != nullptr) {
                        logger->log(game->getCurrentTurn(), player->getUsername(),
                                    "KARTU",
                                    "Kesempatan: " + card->getDescription() +
                                    " -> " + tileLogLabel(target));
                    }
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
            if (logger != nullptr) {
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "KARTU",
                            "Kesempatan: " + card->getDescription() + " -> masuk Penjara");
            }
            gui->showMessage(player->getUsername() + " masuk ke Penjara!");
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
    if (logger != nullptr) {
        logger->log(game->getCurrentTurn(), player->getUsername(),
                    "KARTU", "Dana Umum: " + card->getDescription());
    }

    switch (card->getType()) {
        case CommunityType::BIRTHDAY: {
            for (Player* other : game->getActivePlayers()) {
                if (other == player) continue;
                const int amount = 100;
                bool paid = executePayment(other, player, amount,
                                           "biaya ulang tahun " + formatMoney(amount) +
                                           " kepada " + player->getUsername());
                if (paid) {
                    gui->showMessage(other->getUsername() + " memberi M" + std::to_string(amount) +
                                     " kepada " + player->getUsername() + " (ulang tahun).");
                    if (logger != nullptr) {
                        logger->log(game->getCurrentTurn(), other->getUsername(),
                                    "DANA_UMUM",
                                    "Bayar ulang tahun M" + std::to_string(amount) +
                                    " ke " + player->getUsername());
                    }
                }
            }
            break;
        }
        case CommunityType::DOCTOR_FEE: {
            const int fee = 700;
            bool paid = executePayment(player, nullptr, fee, "biaya dokter " + formatMoney(fee));
            if (paid) {
                gui->showMessage(player->getUsername() + " membayar biaya dokter M" +
                                 std::to_string(fee) + ".");
                if (logger != nullptr) {
                    logger->log(game->getCurrentTurn(), player->getUsername(),
                                "DANA_UMUM", "Bayar biaya dokter " + formatMoney(fee));
                }
            }
            break;
        }
        case CommunityType::CAMPAIGN_FEE: {
            const int fee = 200;
            for (Player* other : game->getActivePlayers()) {
                if (other == player) continue;
                bool paid = executePayment(player, other, fee,
                                           "biaya kampanye " + formatMoney(fee) +
                                           " kepada " + other->getUsername());
                if (!paid) break;
                gui->showMessage(player->getUsername() + " membayar M" + std::to_string(fee) +
                                 " ke " + other->getUsername() + " (biaya kampanye).");
                if (logger != nullptr) {
                    logger->log(game->getCurrentTurn(), player->getUsername(),
                                "DANA_UMUM",
                                "Bayar biaya kampanye " + formatMoney(fee) +
                                " ke " + other->getUsername());
                }
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

        std::istringstream iss(command);
        std::string action;
        iss >> action;
        std::string argument;
        if (iss >> argument) {
            std::string rest;
            std::getline(iss, rest);
            if (!rest.empty()) {
                argument += rest;
            }
        }

        if (action == "NEW_GAME") {
            initNewGame();
            gameLoop();
            break;
        } else if (action == "LOAD_GAME" || action == "MUAT") {
            if (initLoadGame(argument)) {
                gameLoop();
                break;
            }
            gui->loadMainMenu();
            continue;
        } else if (action == "EXIT") {
            break;
        }
    }
}

void GameEngine::initNewGame() {
    resetRuntimeState();
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

    turnManager = new TurnManager(game, dice, gui, logger);
    commandProcessor = new CommandProcessor(this, game, turnManager, dice, gui);
    auctionManager = new AuctionManager(game, logger, gui);
    bankruptcyManager = new BankruptcyManager(game, logger, gui, auctionManager);
    saveLoadManager = new SaveLoadManager(game, logger, turnManager, gui, "data/default");

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

bool GameEngine::initLoadGame(const std::string& requestedPath) {
    std::string filepath = requestedPath;
    if (filepath.empty()) {
        filepath = waitForInput(gui, "Nama file save:");
    }
    if (!loadFromPath(filepath)) {
        gui->showMessage("Gagal memuat permainan.");
        return false;
    }
    return true;
}

void GameEngine::gameLoop() {
    if (game == nullptr || turnManager == nullptr) return;

    while (!game->isGameOver() && !game->isMaxTurnReached()) {
        Player* current = game->getCurrentPlayer();
        if (current == nullptr) break;

        processPlayerTurn(current);

        if (skipAdvanceAfterLoad) {
            skipAdvanceAfterLoad = false;
            continue;
        }

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
    if (resumeLoadedTurn) {
        resumeLoadedTurn = false;
    } else {
        turnManager->startTurn(player);
    }
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
        if (res == CommandResult::LOADED_GAME)    {
            if (performPendingLoad()) {
                return;
            }
            gui->renderPlayer(*player);
            continue;
        }
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
            gui->showMessage("Kamu mendarat di " + prop->getName() +
                             " (" + prop->getCode() + ").");

            if (prop->getType() == PropertyType::STREET) {
                gui->renderProperty(*prop);
                gui->showMessage("Uang kamu saat ini: M" + std::to_string(player->getBalance()));

                int price = prop->getPurchasePrice();
                if (player->getPendingDiscount() > 0) {
                    price = price * (100 - player->getPendingDiscount()) / 100;
                    gui->showMessage("Diskon " + std::to_string(player->getPendingDiscount()) +
                                     "% diterapkan.");
                    gui->showMessage("Harga setelah diskon: M" + std::to_string(price));
                    player->clearPendingDiscount();
                }
                if (player->canAfford(price) &&
                    askYesNo(gui, "Apakah kamu ingin membeli properti ini seharga M" +
                                  std::to_string(price) + "? (y/n):")) {
                    player->deductMoney(price);
                    prop->setOwner(player);
                    prop->setStatus(PropertyStatus::OWNED);
                    player->addProperty(prop);
                    gui->showMessage(prop->getName() + " kini menjadi milikmu.");
                    gui->showMessage("Uang tersisa: M" + std::to_string(player->getBalance()));
                    if (logger) {
                        logger->log(game->getCurrentTurn(), player->getUsername(),
                                    "BELI",
                                    "Beli " + propertyLogLabel(prop) + " seharga M" +
                                    std::to_string(price));
                    }
                } else {
                    if (!player->canAfford(price)) {
                        gui->showMessage("Uang kamu tidak cukup untuk membeli properti ini.");
                    }
                    gui->showMessage("Properti ini akan masuk ke sistem lelang.");
                    if (auctionManager != nullptr) {
                        auctionManager->runAuction(prop, player);
                    }
                }
                return;
            }

            int price = prop->getPurchasePrice();
            if (price > 0 && !player->canAfford(price)) {
                gui->showMessage("Uang kamu tidak cukup untuk mengambil alih " +
                                 prop->getName() + " secara otomatis.");
                gui->showMessage("Properti ini akan masuk ke sistem lelang.");
                if (auctionManager != nullptr) {
                    auctionManager->runAuction(prop, player);
                }
                return;
            }

            if (price > 0) {
                player->deductMoney(price);
            }
            prop->setOwner(player);
            prop->setStatus(PropertyStatus::OWNED);
            player->addProperty(prop);
            if (prop->getType() == PropertyType::RAILROAD) {
                gui->showMessage("Belum ada yang menginjaknya terlebih dahulu.");
                gui->showMessage("Stasiun ini kini menjadi milikmu.");
            } else {
                gui->showMessage("Belum ada yang menginjaknya terlebih dahulu.");
                gui->showMessage(prop->getName() + " kini menjadi milikmu.");
            }
            if (price > 0) {
                gui->showMessage("Uang tersisa: M" + std::to_string(player->getBalance()));
            }
            if (logger) {
                const std::string actionType =
                    prop->getType() == PropertyType::RAILROAD ? "RAILROAD" : "UTILITAS";
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            actionType,
                            propertyLogLabel(prop) + " kini milik " + player->getUsername() +
                            (price > 0 ? " (otomatis, M" + std::to_string(price) + ")" : " (otomatis)"));
            }
            return;
        }

        Player* owner = prop->getOwner();
        if (owner == player) {
            gui->showMessage("Kamu mendarat di propertimu sendiri: " + prop->getName() + ".");
            return;
        }
        if (prop->isMortgaged()) {
            gui->showMessage("Kamu mendarat di " + prop->getName() + " (" + prop->getCode() +
                             "), milik " + owner->getUsername() + ".");
            gui->showMessage("Properti ini sedang digadaikan. Tidak ada sewa yang dikenakan.");
            return;
        }

        int rent = prop->calculateRent(prop->getType() == PropertyType::UTILITY
                                       ? game->getLastDiceTotal()
                                       : 0);
        gui->showMessage("Kamu mendarat di " + prop->getName() + " (" + prop->getCode() +
                         "), milik " + owner->getUsername() + ".");
        std::string condition = rentConditionLabel(prop);
        if (!condition.empty()) {
            gui->showMessage("Kondisi      : " + condition);
        }
        gui->showMessage("Sewa         : M" + std::to_string(rent));
        int payerBefore = player->getBalance();
        int ownerBefore = owner->getBalance();
        bool paid = executePayment(player, owner, rent,
                                   "sewa " + formatMoney(rent) +
                                   " kepada " + owner->getUsername());
        if (paid) {
            if (payerBefore >= rent) {
                gui->showMessage("Uang kamu     : M" + std::to_string(payerBefore) + " -> M"
                                 + std::to_string(player->getBalance()));
                gui->showMessage("Uang " + owner->getUsername() + " : M" + std::to_string(ownerBefore)
                                 + " -> M" + std::to_string(owner->getBalance()));
            }
            if (logger) {
                std::string detail = "Bayar M" + std::to_string(rent) + " ke " +
                                     owner->getUsername() + " (" + prop->getCode();
                if (!condition.empty()) {
                    detail += ", " + condition;
                }
                detail += ")";
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "SEWA", detail);
            }
        }
        return;
    }

    if (auto* incomeTax = dynamic_cast<IncomeTaxTile*>(tile)) {
        gui->showMessage("Kamu mendarat di Pajak Penghasilan (PPH).");
        gui->showMessage("1. Bayar flat M" + std::to_string(incomeTax->getFlatAmount()));
        gui->showMessage("2. Bayar " + std::to_string(incomeTax->getTaxPercentage()) +
                         "% dari total kekayaan (Pilih sebelum menghitung kekayaan!)");
        int choice = askIncomeTaxChoice(gui);
        int amount = 0;
        int before = player->getBalance();
        if (choice == 1) {
            amount = incomeTax->getFlatAmount();
            gui->showMessage("Kamu memilih membayar flat sebesar M" + std::to_string(amount) + ".");
        } else {
            int cash = player->getBalance();
            int propertyValue = player->calculatePropertyAssetValue();
            int buildingValue = player->calculateBuildingAssetValue();
            int wealth = cash + propertyValue + buildingValue;
            amount = (wealth * incomeTax->getTaxPercentage()) / 100;
            gui->showMessage("Total kekayaan kamu:");
            gui->showMessage("- Uang tunai          : M" + std::to_string(cash));
            gui->showMessage("- Harga beli properti : M" + std::to_string(propertyValue));
            gui->showMessage("- Harga beli bangunan : M" + std::to_string(buildingValue));
            gui->showMessage("Total                 : M" + std::to_string(wealth));
            gui->showMessage("Pajak " + std::to_string(incomeTax->getTaxPercentage()) +
                             "%             : M" + std::to_string(amount));
        }
        bool paid = executePayment(player, nullptr, amount,
                                   "Pajak Penghasilan " + formatMoney(amount));
        if (paid) {
            if (before >= amount) {
                gui->showMessage("Uang kamu: M" + std::to_string(before) + " -> M"
                                 + std::to_string(player->getBalance()));
            }
            if (logger) {
                const std::string detail = (choice == 1)
                    ? "PPH flat " + formatMoney(amount)
                    : "PPH " + std::to_string(incomeTax->getTaxPercentage()) +
                      "% = " + formatMoney(amount);
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "PAJAK", detail);
            }
        }
        return;
    }

    if (auto* luxuryTax = dynamic_cast<LuxuryTaxTile*>(tile)) {
        int amount = luxuryTax->getFlatAmount();
        gui->showMessage("Kamu mendarat di Pajak Barang Mewah (PBM).");
        gui->showMessage("Pajak sebesar M" + std::to_string(amount) + " langsung dipotong.");
        int before = player->getBalance();
        bool paid = executePayment(player, nullptr, amount,
                                   "Pajak Barang Mewah " + formatMoney(amount));
        if (paid) {
            if (before >= amount) {
                gui->showMessage("Uang kamu: M" + std::to_string(before) + " -> M"
                                 + std::to_string(player->getBalance()));
            }
            if (logger) {
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "PAJAK", "PBM " + formatMoney(amount));
            }
        }
        return;
    }

    if (dynamic_cast<FestivalTile*>(tile) != nullptr) {
        if (player->getOwnedProperties().empty()) {
            if (logger != nullptr) {
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "FESTIVAL", "Mendarat di Festival, tetapi tidak punya properti");
            }
            gui->showMessage(player->getUsername() +
                             " mendarat di Festival, tetapi tidak memiliki properti.");
            gui->showMessage("Efek Festival hangus.");
            return;
        }
        player->setPendingFestival(true);
        if (logger != nullptr) {
            logger->log(game->getCurrentTurn(), player->getUsername(),
                        "FESTIVAL", "Mendarat di Festival dan harus memilih properti");
        }
        gui->showMessage("Kamu mendarat di Festival.");
        gui->showMessage("Gunakan FESTIVAL <kode_properti> untuk memilih properti yang sewanya akan digandakan.");
        return;
    }

    if (auto* goTile = dynamic_cast<GoTile*>(tile)) {
        gui->showMessage(player->getUsername() + " mendarat tepat di petak GO.");
        gui->showMessage("Kamu menerima M" + std::to_string(goTile->getSalary()) + ".");
        return;
    }

    if (dynamic_cast<GoToJailTile*>(tile) != nullptr) {
        player->setStatus(PlayerStatus::JAILED);
        if (game->getBoard() && game->getBoard()->getJailTile()) {
            player->setPosition(game->getBoard()->getJailTile()->getIndex());
        }
        if (logger != nullptr) {
            logger->log(game->getCurrentTurn(), player->getUsername(),
                        "PENJARA", "Mendarat di Pergi ke Penjara -> masuk Penjara");
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

bool GameEngine::executePayment(Player* from, Player* to, int amount,
                                const std::string& obligationLabel) {
    if (from == nullptr || amount <= 0) return false;
    if (turnManager != nullptr && turnManager->isShieldActive()) {
        gui->showMessage(from->getUsername() + " terlindungi oleh Perisai.");
        gui->showMessage("Kewajiban " + obligationLabel + " dibatalkan.");
        return true;
    }
    if (bankruptcyManager != nullptr) {
        return bankruptcyManager->handleInsufficientFunds(*from, amount, to, obligationLabel);
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

    const auto& active = game->getActivePlayers();
    if (active.empty()) { gui->loadFinishMenu(); return; }

    // Multi-criteria: totalWealth desc → property count desc → card count desc → all tied = everyone wins
    int bestWealth = -1;
    for (Player* p : active) bestWealth = std::max(bestWealth, p->calculateTotalWealth());

    std::vector<Player*> candidates;
    for (Player* p : active)
        if (p->calculateTotalWealth() == bestWealth) candidates.push_back(p);

    if (candidates.size() > 1) {
        int bestProps = -1;
        for (Player* p : candidates) bestProps = std::max(bestProps, (int)p->getOwnedProperties().size());
        std::vector<Player*> next;
        for (Player* p : candidates)
            if ((int)p->getOwnedProperties().size() == bestProps) next.push_back(p);
        candidates = next;
    }

    if (candidates.size() > 1) {
        int bestCards = -1;
        for (Player* p : candidates) bestCards = std::max(bestCards, p->getCardCount());
        std::vector<Player*> next;
        for (Player* p : candidates)
            if (p->getCardCount() == bestCards) next.push_back(p);
        candidates = next;
    }

    gui->loadFinishMenu();
    if (candidates.size() == 1) {
        gui->renderWinner(*candidates[0]);
    } else {
        for (Player* p : candidates) gui->renderWinner(*p);
    }
}
