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
#include <set>

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
#include "utils/Formatter.hpp"

#include "exception/GameException.hpp"
#include "exception/PlayerTurnException.hpp"
#include "exception/InvalidEntryInputException.hpp"
#include "exception/InvalidFileException.hpp"
#include "exception/PlayerTurn/PropertyManagementException.hpp"
#include "exception/PlayerTurn/PropertyManagement/PropertyNotMortgagedException.hpp"
#include "exception/PlayerTurn/PropertyManagement/InsufficientOwnedColorException.hpp"
#include "exception/PlayerTurn/PropertyManagement/InsufficientMoneyException.hpp"
#include "exception/PlayerTurn/PropertyManagement/ExistingHotelException.hpp"
#include "exception/PlayerTurn/InvalidTurnException.hpp"
#include "exception/PlayerTurn/SkillTurnException.hpp"
#include "exception/PlayerTurn/SkillTurn/DiceAlreadyRolledException.hpp"
#include "exception/PlayerTurn/SkillTurn/SkillCardUsedException.hpp"
#include "exception/InvalidEntryInput/InvalidDiceNumberException.hpp"
#include "exception/InvalidEntryInput/InvalidTileException.hpp"
#include "exception/InvalidFile/FailedToSaveException.hpp"
#include "exception/InvalidFile/FileNotFoundException.hpp"
#include "exception/InvalidFile/InvalidConfigException.hpp"
#include "exception/InvalidFile/UnreadableFileException.hpp"

namespace
{
    constexpr const char *kDefaultConfigDir = "data/config/default";

    std::string tileLogLabel(const Tile *tile)
    {
        if (tile == nullptr)
            return "?";
        if (tile->getCode().empty())
            return tile->getName();
        return tile->getName() + " (" + tile->getCode() + ")";
    }

    std::string propertyLogLabel(const Property *property)
    {
        if (property == nullptr)
            return "?";
        return property->getName() + " (" + property->getCode() + ")";
    }

    std::mt19937 makeRandomEngine()
    {
        std::random_device rd;
        const auto now = static_cast<unsigned int>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count());
        std::seed_seq seed{rd(), rd(), rd(), rd(), now};
        return std::mt19937(seed);
    }

    std::string buildingStateLabel(const StreetProperty *property)
    {
        if (property == nullptr)
            return "tanah kosong";

        switch (property->getBuildingState())
        {
        case BuildingState::NONE:
            return "tanah kosong";
        case BuildingState::HOUSE_1:
            return "1 rumah";
        case BuildingState::HOUSE_2:
            return "2 rumah";
        case BuildingState::HOUSE_3:
            return "3 rumah";
        case BuildingState::HOUSE_4:
            return "4 rumah";
        case BuildingState::HOTEL:
            return "Hotel";
        default:
            return "tanah kosong";
        }
    }

    std::string rentConditionLabel(const Property *property)
    {
        if (property == nullptr)
            return "";

        std::string label;
        if (property->isStreet())
        {
            auto *street = static_cast<const StreetProperty *>(property);
            label = buildingStateLabel(street);
        }
        else if (property->getType() == PropertyType::RAILROAD)
        {
            label = "Railroad";
        }
        else if (property->getType() == PropertyType::UTILITY)
        {
            label = "Utility";
        }

        if (property->getFestivalDuration() > 0)
        {
            if (!label.empty())
                label += " | ";
            label += "Festival aktif x" + std::to_string(property->getFestivalMultiplier()) +
                     " (" + std::to_string(property->getFestivalDuration()) + " giliran)";
        }
        return label;
    }
}

GameEngine::GameEngine(IGUI *gui)
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

GameEngine::~GameEngine()
{
    resetRuntimeState();
    delete dice;
    delete logger;
}

void GameEngine::resetRuntimeState()
{
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

void GameEngine::requestLoad(const std::string &filepath)
{
    pendingLoadRequested = true;
    pendingLoadPath = filepath;
}

bool GameEngine::performPendingLoad()
{
    if (!pendingLoadRequested)
        return true;
    const std::string filepath = pendingLoadPath;
    pendingLoadRequested = false;
    pendingLoadPath.clear();
    const bool ok = loadFromPath(filepath);
    if (ok)
    {
        skipAdvanceAfterLoad = true;
    }
    return ok;
}

bool GameEngine::loadFromPath(const std::string &filepath)
{
    std::string configDir = kDefaultConfigDir;
    {
        namespace fs = std::filesystem;
        fs::path direct(filepath);
        fs::path saveDir = direct.has_parent_path() ? direct : (fs::path("data") / direct);
        if (fs::exists(saveDir) && fs::is_directory(saveDir))
        {
            configDir = saveDir.string();
        }
    }

    ConfigLoader loader(configDir);
    GameConfig config = loader.loadGameConfig();

    Game *loadedGame = new Game();
    loadedGame->setConfigValues(
        config.getMisc().getMaxTurn(),
        config.getMisc().getInitialBalance(),
        config.getSpecial().getGoSalary(),
        config.getSpecial().getJailFine(),
        config.getTax().getPphFlat(),
        config.getTax().getPphPercent(),
        config.getTax().getPbmFlat(),
        config.getRailroadRents(),
        config.getUtilityMultipliers());
    loadedGame->setBoard(loader.buildBoard(config.getProperties(), config));
    auto decks = loader.buildDecks();
    loadedGame->setDecks(std::get<0>(decks), std::get<1>(decks), std::get<2>(decks));

    TurnManager *loadedTurnManager = new TurnManager(loadedGame, dice, gui);
    TransactionLogger loadedLogger;
    SaveLoadManager *loaderManager =
        new SaveLoadManager(loadedGame, &loadedLogger, loadedTurnManager, gui, configDir);

    if (!loaderManager->load(filepath))
    {
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

    if (logger != nullptr)
    {
        logger->log(game->getCurrentTurn(), "SISTEM",
                    "MUAT", "Game dimuat dari " + filepath);
        gui->renderLog(logger->getFullLog(), "LOG");
    }
    gui->showMessage("Permainan berhasil dimuat dari " + filepath + ".");
    gui->renderBoard(*game);
    gui->loadGameView();

    for (Player *p : game->getPlayers())
    {
        gui->renderPlayer(*p);
    }

    resumeLoadedTurn = true;
    return true;
}

std::string GameEngine::waitForInput(IGUI *gui, const std::string &prompt)
{
    gui->showInputPrompt(prompt);
    while (!gui->shouldExit())
    {
        gui->update();
        gui->display();
        std::string c = gui->getCommand();
        if (c != "NULL")
            return c;
    }
    return "";
}

void GameEngine::waitForResume(IGUI *gui)
{
    if (gui == nullptr)
        return;

    gui->setResumeVisible(true);
    while (!gui->shouldExit())
    {
        gui->update();
        gui->display();

        std::string cmd = gui->getCommand();
        if (cmd == "RESUME")
            break;
    }
    gui->setResumeVisible(false);
}

std::string GameEngine::normalizeInput(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c)
                   { return std::toupper(c); });
    return s;
}

bool GameEngine::askYesNo(IGUI *gui, const std::string &prompt)
{
    gui->showConfirm(prompt);

    while (!gui->shouldExit())
    {
        gui->update();
        gui->display();

        std::string cmd = gui->getCommand();
        if (cmd == "YES")
            return true;
        if (cmd == "NO")
            return false;
    }
    return false;
}

int GameEngine::askIncomeTaxChoice(IGUI *gui)
{
    bool choice = askYesNo(
        gui,
        "Bayar pajak flat?\n\nYES = Flat\nNO = Persentase");

    return choice ? 1 : 2;
}

CommandResult GameEngine::resolveRoll(Player *player, bool manual, int d1, int d2, bool fromJailAttempt)
{
    if (player == nullptr || turnManager == nullptr || dice == nullptr || gui == nullptr)
    {
        return CommandResult::INVALID;
    }

    if (!turnManager->canRoll(player))
    {
        gui->showMessage("Kamu belum bisa melempar dadu sekarang.");
        return CommandResult::INVALID;
    }

    if (fromJailAttempt)
    {
        gui->showMessage("Mencoba keluar dari Penjara dengan melempar dadu.");
    }

    if (manual)
    {
        gui->showMessage("Dadu diatur secara manual.");
        dice->setManual(d1, d2);
    }
    else
    {
        dice->rollRandom();
    }

    int total = dice->getTotal();
    bool rolledDouble = dice->isDouble();
    gui->renderDice(dice->getDie1(), dice->getDie2());
    game->setLastDiceTotal(total);
    waitForResume(gui);

    if (fromJailAttempt && player->isJailed())
    {
        player->markRolled();
        turnManager->setPhase(TurnPhase::POST_ROLL);
        turnManager->markActed();

        if (!rolledDouble)
        {
            player->incrementJailAttempts();
            if (logger != nullptr)
            {
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "DADU",
                            "Lempar: " + std::to_string(dice->getDie1()) + "+" +
                                std::to_string(dice->getDie2()) + "=" + std::to_string(total) +
                                " -> gagal keluar dari Penjara (" +
                                std::to_string(player->getJailAttempts()) + "/3)");
                gui->renderLog(logger->getFullLog(), "LOG");
            }
            gui->showMessage("Kamu belum mendapatkan double.");
            gui->showMessage("Kamu tetap berada di Penjara dan tidak bergerak.");
            gui->showMessage("Percobaan keluar dari Penjara: " +
                             std::to_string(player->getJailAttempts()) + "/3.");
            return CommandResult::END_TURN;
        }

        if (logger != nullptr)
        {
            logger->log(game->getCurrentTurn(), player->getUsername(),
                        "PENJARA", "Bebas dari Penjara dengan double");
            gui->renderLog(logger->getFullLog(), "LOG");
        }
        gui->showMessage("Double! Kamu bebas dari Penjara.");
        player->setStatus(PlayerStatus::ACTIVE);
        player->resetJailAttempts();
    }
    else
    {
        player->markRolled();
    }

    if (rolledDouble)
    {
        player->incrementConsecutiveDoubles();
        if (player->getConsecutiveDoubles() >= 3)
        {
            Board *board = game->getBoard();
            if (board != nullptr && board->getJailTile() != nullptr)
            {
                player->setPosition(board->getJailTile()->getIndex());
            }
            player->setStatus(PlayerStatus::JAILED);
            player->resetJailAttempts();
            if (logger != nullptr)
            {
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "DADU",
                            "Lempar: " + std::to_string(dice->getDie1()) + "+" +
                                std::to_string(dice->getDie2()) + "=" + std::to_string(total) +
                                " (double ketiga) -> masuk Penjara");
                gui->renderLog(logger->getFullLog(), "LOG");
            }
            gui->showMessage("Kamu mendapatkan double tiga kali berturut-turut.");
            gui->showMessage("Bidak tidak digerakkan dan kamu langsung masuk Penjara.");
            turnManager->endTurn(player);
            return CommandResult::END_TURN;
        }
    }
    else
    {
        player->resetConsecutiveDoubles();
    }

    Tile *landed = turnManager->processMovement(player, total);
    turnManager->setPhase(TurnPhase::POST_ROLL);
    turnManager->markActed();

    std::string landedName = landed ? landed->getName() : "?";
    gui->renderMovement(player->getUsername(), total, landedName);
    if (logger != nullptr)
    {
        std::string detail = "Lempar: " + std::to_string(dice->getDie1()) + "+" +
                             std::to_string(dice->getDie2()) + "=" + std::to_string(total);
        if (rolledDouble)
        {
            detail += " (double)";
        }
        detail += " -> mendarat di " + tileLogLabel(landed);
        logger->log(game->getCurrentTurn(), player->getUsername(), "DADU", detail);
        gui->renderLog(logger->getFullLog(), "LOG");
    }

    if (landed)
        handleTileLanding(player, landed);

    if (player->getStatus() == PlayerStatus::JAILED)
    {
        gui->showMessage("Giliranmu berakhir karena kamu berada di Penjara.");
        turnManager->endTurn(player);
        return CommandResult::END_TURN;
    }

    if (player->getStatus() == PlayerStatus::BANKRUPT)
    {
        if (checkWinCondition())
            return CommandResult::GAME_OVER;
        return CommandResult::CONTINUE;
    }

    if (player->hasPendingFestival())
    {
        gui->showMessage("Selesaikan dulu efek Festival");
    }
    else if (rolledDouble)
    {
        if (logger != nullptr)
        {
            std::string detail;
            if (fromJailAttempt)
            {
                detail = "Mendapat lemparan bonus setelah bebas dari Penjara";
            }
            else
            {
                detail = "Giliran tambahan ke-" +
                         std::to_string(player->getConsecutiveDoubles());
            }
            logger->log(game->getCurrentTurn(), player->getUsername(), "DOUBLE", detail);
            gui->renderLog(logger->getFullLog(), "LOG");
        }
        if (fromJailAttempt)
        {
            gui->showMessage("Karena berhasil keluar dengan double, kamu juga mendapat lemparan bonus.");
        }
        else
        {
            gui->showMessage("Dadu menunjukkan double. Kamu mendapat lemparan bonus.");
        }
    }
    else
    {
        gui->showMessage("Aksi lempar dadu selesai.");
    }

    return CommandResult::CONTINUE;
}

CommandResult GameEngine::handleJailedPlayerTurn(Player *player)
{
    if (player == nullptr || !player->isJailed())
    {
        return CommandResult::CONTINUE;
    }

    const int fine = game->getJailFine();

    while (!gui->shouldExit())
    {
        gui->showMessage(player->getUsername() + " sedang berada di Penjara.");

        if (player->getJailAttempts() >= 3)
        {
            gui->showMessage("Ini adalah giliran ke-4 kamu di Penjara.");
            gui->showMessage("Kamu wajib membayar denda " + Formatter::money(fine) + " sebelum melempar dadu.");
            bool paid = executePayment(player, nullptr, fine,
                                       "denda penjara " + Formatter::money(fine));
            if (paid)
            {
                player->setStatus(PlayerStatus::ACTIVE);
                player->resetJailAttempts();
                if (logger != nullptr)
                {
                    logger->log(game->getCurrentTurn(), player->getUsername(),
                                "PENJARA",
                                "Bayar denda " + Formatter::money(fine) + " dan bebas dari Penjara");
                    gui->renderLog(logger->getFullLog(), "LOG");
                }
                gui->showMessage("Denda penjara sudah dibayar. Kamu bebas dari Penjara.");
                gui->showMessage("Sekarang kamu dapat melempar dadu seperti biasa.");
                return CommandResult::CONTINUE;
            }
            if (player->getStatus() == PlayerStatus::BANKRUPT && checkWinCondition())
            {
                return CommandResult::GAME_OVER;
            }
            return CommandResult::END_TURN;
        }

        SkillCard *jailCard = player->findJailFreeCard();
        gui->showMessage("Pilih cara untuk keluar dari Penjara:");
        gui->showMessage("1) Bayar denda " + Formatter::money(fine));
        gui->showMessage("2) Gunakan kartu Bebas dari Penjara" +
                         std::string(jailCard ? "" : " (tidak tersedia)"));
        gui->showMessage("3) Coba lempar dadu dan harus mendapatkan double");

        std::string choice = normalizeInput(waitForInput(gui, "Opsi penjara (1/2/3):"));

        if (choice == "1")
        {
            bool paid = executePayment(player, nullptr, fine,
                                       "denda penjara " + Formatter::money(fine));
            if (paid)
            {
                player->setStatus(PlayerStatus::ACTIVE);
                player->resetJailAttempts();
                gui->showMessage("Denda penjara sudah dibayar. Kamu bebas dari Penjara.");
                gui->showMessage("Sekarang kamu dapat melempar dadu seperti biasa.");
                return CommandResult::CONTINUE;
            }
            if (player->getStatus() == PlayerStatus::BANKRUPT && checkWinCondition())
            {
                return CommandResult::GAME_OVER;
            }
            return CommandResult::END_TURN;
        }

        if (choice == "2")
        {
            if (jailCard == nullptr)
            {
                gui->showMessage("Kamu tidak memiliki kartu Bebas dari Penjara.");
                continue;
            }
            player->removeCard(jailCard);
            if (game->getSkillDeck() != nullptr)
            {
                game->getSkillDeck()->discard(jailCard);
            }
            player->setStatus(PlayerStatus::ACTIVE);
            player->resetJailAttempts();
            if (logger != nullptr)
            {
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "KARTU",
                            "Pakai " + jailCard->getCardName() + " -> bebas dari Penjara");
                gui->renderLog(logger->getFullLog(), "LOG");
            }
            gui->showMessage("Kartu Bebas dari Penjara digunakan. Kamu bebas dari Penjara.");
            gui->showMessage("Sekarang kamu dapat melempar dadu seperti biasa.");
            return CommandResult::CONTINUE;
        }

        if (choice == "3")
        {
            while (!gui->shouldExit())
            {
                std::string raw = waitForInput(gui, "Perintah penjara:");
                if (raw.empty() || raw == "NULL")
                    continue;

                std::istringstream iss(raw);
                std::vector<std::string> tokens;
                std::string tok;
                while (iss >> tok)
                    tokens.push_back(tok);
                if (tokens.empty())
                    continue;

                std::string cmd = normalizeInput(tokens[0]);

                if (cmd == "HELP")
                {
                    gui->showMessage("Saat berada di Penjara, pilih salah satu cara keluar:");
                    gui->showMessage("1) Bayar denda, 2) Gunakan kartu Bebas dari Penjara, 3) Coba lempar dadu.");
                    gui->showMessage("Untuk opsi lempar dadu, gunakan LEMPAR_DADU atau ATUR_DADU <x> <y>.");
                    continue;
                }

                if (cmd == "LEMPAR_DADU")
                {
                    return resolveRoll(player, false, 0, 0, true);
                }

                if (cmd == "ATUR_DADU")
                {
                    if (tokens.size() < 3)
                    {
                        continue;
                    }
                    int d1 = 0, d2 = 0;
                    try
                    {
                        d1 = std::stoi(tokens[1]);
                        d2 = std::stoi(tokens[2]);
                    }
                    catch (...)
                    {
                        throw InvalidDiceNumberException(d1, d2);
                    }
                    if (d1 < 1 || d1 > 6 || d2 < 1 || d2 > 6)
                    {
                        gui->showMessage("Nilai dadu harus berada pada rentang 1 sampai 6.");
                        continue;
                    }
                    return resolveRoll(player, true, d1, d2, true);
                }

                if (cmd == "CETAK_PAPAN" || cmd == "CETAK_AKTA" || cmd == "CETAK_DEED" || cmd == "CETAK_PROPERTI" ||
                    cmd == "CETAK_LOG" || cmd == "SIMPAN" || cmd == "MUAT" || cmd == "EXIT")
                {
                    CommandResult res = commandProcessor->process(raw, player);
                    if (res == CommandResult::GAME_OVER || res == CommandResult::SAVED_MID_TURN || res == CommandResult::LOADED_GAME)
                    {
                        return res;
                    }
                    continue;
                }
            }
        }

        gui->showMessage("Pilihan tidak valid. Masukkan 1, 2, atau 3.");
    }

    game->setGameOver(true);
    return CommandResult::GAME_OVER;
}

void GameEngine::handleChanceLanding(Player *player, ChanceTile * /*tile*/)
{
    if (player == nullptr || game == nullptr || game->getChanceDeck() == nullptr ||
        game->getChanceDeck()->isEmpty())
    {
        return;
    }

    ChanceCard *card = game->getChanceDeck()->draw();
    if (card == nullptr)
        return;

    gui->showMessage(player->getUsername() + " mengambil kartu Kesempatan: " + card->getDescription());

    Board *board = game->getBoard();
    switch (card->getType())
    {
    case ChanceType::GO_TO_NEAREST_STATION:
    {
        if (board != nullptr)
        {
            RailroadTile *nearest = board->getNearestRailroad(player->getPosition());
            if (nearest != nullptr)
            {
                player->setPosition(nearest->getIndex());
                if (logger != nullptr)
                {
                    logger->log(game->getCurrentTurn(), player->getUsername(),
                                "KARTU",
                                "Kesempatan: " + card->getDescription() +
                                    " -> " + tileLogLabel(nearest));
                    gui->renderLog(logger->getFullLog(), "LOG");
                }
                gui->showMessage(player->getUsername() + " berpindah ke stasiun terdekat.");
                gui->showMessage("Bidak mendarat di: " + nearest->getName() + ".");
                handleTileLanding(player, nearest);
            }
        }
        break;
    }
    case ChanceType::MOVE_BACK_3:
    {
        if (board != nullptr)
        {
            const int boardSize = board->getSize() > 0 ? board->getSize() : 40;
            const int newPos = (player->getPosition() - 3 + boardSize) % boardSize;
            player->setPosition(newPos);
            Tile *target = board->getTile(newPos);
            gui->showMessage(player->getUsername() + " mundur 3 petak.");
            if (target != nullptr)
            {
                if (logger != nullptr)
                {
                    logger->log(game->getCurrentTurn(), player->getUsername(),
                                "KARTU",
                                "Kesempatan: " + card->getDescription() +
                                    " -> " + tileLogLabel(target));
                    gui->renderLog(logger->getFullLog(), "LOG");
                }
                gui->showMessage("Bidak mendarat di: " + target->getName() + ".");
                handleTileLanding(player, target);
            }
        }
        break;
    }
    case ChanceType::GO_TO_JAIL:
    {
        player->setStatus(PlayerStatus::JAILED);
        player->resetJailAttempts();
        if (board != nullptr && board->getJailTile() != nullptr)
        {
            player->setPosition(board->getJailTile()->getIndex());
        }
        if (logger != nullptr)
        {
            logger->log(game->getCurrentTurn(), player->getUsername(),
                        "KARTU",
                        "Kesempatan: " + card->getDescription() + " -> masuk Penjara");
            gui->renderLog(logger->getFullLog(), "LOG");
        }
        gui->showMessage(player->getUsername() + " masuk ke Penjara!");
        break;
    }
    }

    game->getChanceDeck()->discard(card);
}

void GameEngine::handleCommunityChestLanding(Player *player, CommunityChestTile * /*tile*/)
{
    if (player == nullptr || game == nullptr || game->getCommunityDeck() == nullptr ||
        game->getCommunityDeck()->isEmpty())
    {
        return;
    }

    CommunityChestCard *card = game->getCommunityDeck()->draw();
    if (card == nullptr)
        return;

    gui->showMessage(player->getUsername() + " mengambil kartu Dana Umum: " + card->getDescription());
    if (logger != nullptr)
    {
        logger->log(game->getCurrentTurn(), player->getUsername(),
                    "KARTU", "Dana Umum: " + card->getDescription());
        gui->renderLog(logger->getFullLog(), "LOG");
    }

    switch (card->getType())
    {
    case CommunityType::BIRTHDAY:
    {
        for (Player *other : game->getActivePlayers())
        {
            if (other == player)
                continue;
            const int amount = 100;
            bool paid = executePayment(other, player, amount,
                                       "biaya ulang tahun " + Formatter::money(amount) +
                                           " kepada " + player->getUsername());
            if (paid)
            {
                gui->showMessage(other->getUsername() + " memberi " + Formatter::money(amount) +
                                 " kepada " + player->getUsername() + " (ulang tahun).");
                if (logger != nullptr)
                {
                    logger->log(game->getCurrentTurn(), other->getUsername(),
                                "DANA_UMUM",
                                "Bayar ulang tahun " + Formatter::money(amount) +
                                    " ke " + player->getUsername());
                    gui->renderLog(logger->getFullLog(), "LOG");
                }
            }
        }
    case CommunityType::DOCTOR_FEE:
    {
        const int fee = 700;
        bool paid = executePayment(player, nullptr, fee, "biaya dokter " + Formatter::money(fee));
        if (paid)
        {
            gui->showMessage(player->getUsername() + " membayar biaya dokter " + Formatter::money(fee) + ".");
            if (logger != nullptr)
            {
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "DANA_UMUM", "Bayar biaya dokter " + Formatter::money(fee));
                gui->renderLog(logger->getFullLog(), "LOG");
            }
        }
    }
    case CommunityType::CAMPAIGN_FEE:
    {
        const int fee = 200;
        for (Player *other : game->getActivePlayers())
        {
            if (other == player)
                continue;
            bool paid = executePayment(player, other, fee,
                                       "biaya kampanye " + Formatter::money(fee) +
                                           " kepada " + other->getUsername());
            if (!paid)
                break;
            gui->showMessage(player->getUsername() + " membayar " + Formatter::money(fee) +
                             " ke " + other->getUsername() + " (biaya kampanye).");
            if (logger != nullptr)
            {
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "DANA_UMUM",
                            "Bayar biaya kampanye " + Formatter::money(fee) +
                                " ke " + other->getUsername());
                gui->renderLog(logger->getFullLog(), "LOG");
            }
        }
        break;
    }
    }

        game->getCommunityDeck()->discard(card);
    }
}

void GameEngine::run()
{
    if (gui == nullptr)
    {
        return;
    }

    gui->loadMainMenu();

    // Loop menu utama — menunggu user memilih NEW_GAME atau LOAD_GAME
    while (!gui->shouldExit())
    {
        gui->update();

        if (gui->shouldExit())
            break;

        gui->display();

        std::string command = gui->getCommand();
        if (command == "NULL" || command.empty())
        {
            continue;
        }

        std::istringstream iss(command);
        std::string action;
        iss >> action;
        action = normalizeInput(action);
        std::string argument;
        if (iss >> argument)
        {
            std::string rest;
            std::getline(iss, rest);
            if (!rest.empty())
            {
                argument += rest;
            }
        }
        try
        {
            if (action == "NEW_GAME")
            {
                initNewGame(argument);
                gameLoop();
                break;
            }
            else if (action == "LOAD_GAME" || action == "MUAT")
            {
                if (initLoadGame(argument))
                {
                    gameLoop();
                    break;
                }
                gui->loadMainMenu();
                continue;
            }
            else if (action == "EXIT")
            {
                break;
            }
        }
        catch (const GameException &e)
        {
            gui->loadMainMenu();
            gui->showException(e.getErrorCode(), e.what());
        }
        catch (const std::exception &e)
        {
            gui->loadMainMenu();
            gui->showException(500, e.what());
        }
    }
}

void GameEngine::initNewGame(const std::string &configPath)
{
    resetRuntimeState();
    this->game = new Game();

    std::string base = "data/config/";
    std::string name = configPath.empty() ? "default" : configPath;
    std::string path = base + name;

    ConfigLoader loader(path);
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
        config.getUtilityMultipliers());

    game->setBoard(loader.buildBoard(config.getProperties(), config));

    auto decks = loader.buildDecks();
    game->setDecks(std::get<0>(decks), std::get<1>(decks), std::get<2>(decks));

    turnManager = new TurnManager(game, dice, gui, logger);
    commandProcessor = new CommandProcessor(this, game, turnManager, dice, gui);
    auctionManager = new AuctionManager(game, logger, gui);
    bankruptcyManager = new BankruptcyManager(game, logger, gui, auctionManager);
    saveLoadManager = new SaveLoadManager(game, logger, turnManager, gui, kDefaultConfigDir);

    std::string s = waitForInput(gui, "Jumlah pemain (2-4):");
    if (s.empty())
        throw GameException(400, "Jumlah pemain tidak boleh kosong.");

    int numPlayers;
    try
    {
        numPlayers = std::stoi(s);
    }
    catch (...)
    {
        throw GameException(400, "Jumlah pemain harus berupa angka.");
    }
    if (numPlayers < 2 || numPlayers > 4)
        throw GameException(400, "Jumlah pemain harus 2-4.");

    int initBalance = config.getMisc().getInitialBalance();
    for (int i = 0; i < numPlayers; ++i)
    {
        std::string uname = waitForInput(gui,
                                         "Username pemain ke-" + std::to_string(i + 1) + ":");

        auto isBlank = [](const std::string &s)
        {
            return std::all_of(s.begin(), s.end(), ::isspace);
        };

        if (uname.empty() || isBlank(uname))
        {
            uname = "Player" + std::to_string(i + 1);
        }

        game->addPlayer(new Player(uname, initBalance));
    }

    std::vector<int> order(numPlayers);
    std::iota(order.begin(), order.end(), 0);
    game->setTurnOrder(order);
    game->setCurrentTurnIndex(0);
    game->setCurrentTurn(1);

    gui->showMessage("Urutan giliran sudah diacak.");
    for (int i = 0; i < game->getActivePlayerCount(); ++i)
    {
        gui->showMessage(std::to_string(i + 1) + ". " + game->getActivePlayers().at(i)->getUsername());
    }

    Board *b = game->getBoard();
    int goIndex = (b && b->getGoTile()) ? b->getGoTile()->getIndex() : 0;

    for (Player *p : game->getPlayers())
    {
        p->setPosition(goIndex);
    }

    gui->renderBoard(*game);
    gui->loadGameView();

    for (Player *p : game->getPlayers())
    {
        gui->renderPlayer(*p);
    }
}

bool GameEngine::initLoadGame(const std::string &requestedPath)
{
    std::string filepath = requestedPath;
    if (filepath.empty())
    {
        filepath = waitForInput(gui, "Nama file save:");
    }
    return loadFromPath(filepath);
}

void GameEngine::gameLoop()
{
    if (game == nullptr || turnManager == nullptr)
        return;

    while (!game->isGameOver() && !game->isMaxTurnReached())
    {
        Player *current = game->getCurrentPlayer();
        if (current == nullptr)
            break;

        processPlayerTurn(current);

        if (skipAdvanceAfterLoad)
        {
            skipAdvanceAfterLoad = false;
            continue;
        }

        if (checkWinCondition())
        {
            game->setGameOver(true);
            break;
        }

        game->advanceTurnOrder();
        if (game->getCurrentTurnIndex() == 0)
        {
            game->incrementTurn();
        }
    }
    endGame();
}

void GameEngine::processPlayerTurn(Player *player)
{
    const bool resumedLoadedTurn = resumeLoadedTurn;
    if (resumedLoadedTurn)
    {
        resumeLoadedTurn = false;
    }
    else
    {
        turnManager->startTurn(player);
    }
    gui->renderPlayer(*player);
    gui->renderOwnedProperties(*player);
    gui->renderSkillHand(player->getHandCards());
    if (!resumedLoadedTurn)
    {
        turnManager->drawSkillCardForTurn(player);
        gui->renderSkillHand(player->getHandCards());
    }

    if (player->isJailed())
    {
        CommandResult jailResult = handleJailedPlayerTurn(player);
        if (jailResult == CommandResult::END_TURN)
        {
            if (turnManager->getPhase() != TurnPhase::ENDED)
            {
                turnManager->endTurn(player);
            }
            return;
        }
        if (jailResult == CommandResult::GAME_OVER)
        {
            game->setGameOver(true);
            return;
        }
        if (jailResult == CommandResult::SAVED_MID_TURN)
        {
            return;
        }
        gui->renderPlayer(*player);
    }

    while (!gui->shouldExit())
    {
        gui->update();
        gui->display();

        std::string cmd = gui->getCommand();
        if (cmd.empty() || cmd == "NULL")
            continue;

        std::cout << cmd << std::endl;
        if (cmd == "PAUSE")
        {
            gui->showPauseMenu();
            continue;
        }
        if (cmd == "PAUSE_RESUME")
        {
            continue;
        }
        if (cmd == "PAUSE_SAVE")
        {
            std::string path = waitForInput(gui, "Masukkan nama file save:");

            if (!path.empty())
            {
                try
                {
                    saveLoadManager->save(path);
                    gui->showMessage("Game berhasil disimpan ke " + path);
                }
                catch (...)
                {
                    gui->showMessage("Gagal menyimpan game.");
                }
            }
            continue;
        }
        if (cmd == "PAUSE_EXIT")
        {
            game->setGameOver(true);
            return;
        }

        CommandResult res = commandProcessor->process(cmd, player);
        if (res == CommandResult::END_TURN)
            break;
        if (res == CommandResult::LOADED_GAME)
        {
            if (performPendingLoad())
            {
                return;
            }
            gui->renderPlayer(*player);
            continue;
        }
        if (res == CommandResult::GAME_OVER)
        {
            game->setGameOver(true);
            return;
        }
        if (res == CommandResult::SAVED_MID_TURN)
            return;
    }

    if (turnManager->getPhase() != TurnPhase::ENDED)
    {
        turnManager->endTurn(player);
    }
}

void GameEngine::handleTileLanding(Player *player, Tile *tile)
{
    if (player == nullptr || tile == nullptr || game == nullptr)
    {
        return;
    }

    switch (tile->getKind())
    {
    case TileKind::CHANCE:
        handleChanceLanding(player, static_cast<ChanceTile *>(tile));
        return;
    case TileKind::COMMUNITY_CHEST:
        handleCommunityChestLanding(player, static_cast<CommunityChestTile *>(tile));
        return;
    default:
        break;
    }

    if (tile->getCategory() == TileCategory::PROPERTY)
    {
        auto *pt = static_cast<PropertyTile *>(tile);
        Property *prop = pt->getProperty();
        if (prop == nullptr)
            return;

        if (prop->getStatus() == PropertyStatus::BANK)
        {
            gui->showMessage("Kamu mendarat di " + prop->getName() +
                             " (" + prop->getCode() + ").");

            if (prop->getType() == PropertyType::STREET)
            {
                gui->renderProperty(*prop);
                gui->showMessage("Uang kamu saat ini: " + Formatter::money(player->getBalance()));

                int price = prop->getPurchasePrice();
                if (player->getPendingDiscount() > 0)
                {
                    price = price * (100 - player->getPendingDiscount()) / 100;
                    gui->showMessage("Diskon " + std::to_string(player->getPendingDiscount()) +
                                     "% diterapkan.");
                    gui->showMessage("Harga setelah diskon: " + Formatter::money(price));
                    player->clearPendingDiscount();
                }
                if (player->canAfford(price) &&
                    askYesNo(gui, "Apakah kamu ingin membeli properti ini seharga " + Formatter::money(price) + "?"))
                {
                    player->deductMoney(price);
                    prop->setOwner(player);
                    prop->setStatus(PropertyStatus::OWNED);
                    player->addProperty(prop);
                    gui->showMessage(prop->getName() + " kini menjadi milikmu.");
                    gui->showMessage("Uang tersisa: " + Formatter::money(player->getBalance()));
                    if (logger)
                    {
                        logger->log(game->getCurrentTurn(), player->getUsername(),
                                    "BELI",
                                    "Beli " + propertyLogLabel(prop) + " seharga " + Formatter::money(price));
                        gui->renderLog(logger->getFullLog(), "LOG");
                    }
                }
                else
                {
                    if (!player->canAfford(price))
                    {
                        gui->showMessage("Uang kamu tidak cukup untuk membeli properti ini.");
                    }
                    gui->showMessage("Properti ini akan masuk ke sistem lelang.");
                    if (auctionManager != nullptr)
                    {
                        auctionManager->runAuction(prop, player);
                    }
                }
                return;
            }

            int price = prop->getPurchasePrice();
            if (price > 0 && !player->canAfford(price))
            {
                gui->showMessage("Uang kamu tidak cukup untuk mengambil alih " +
                                 prop->getName() + " secara otomatis.");
                gui->showMessage("Properti ini akan masuk ke sistem lelang.");
                if (auctionManager != nullptr)
                {
                    auctionManager->runAuction(prop, player);
                }
                return;
            }

            if (price > 0)
            {
                player->deductMoney(price);
            }
            prop->setOwner(player);
            prop->setStatus(PropertyStatus::OWNED);
            player->addProperty(prop);
            if (prop->getType() == PropertyType::RAILROAD)
            {
                gui->showMessage("Belum ada yang menginjaknya terlebih dahulu.");
                gui->showMessage("Stasiun ini kini menjadi milikmu.");
            }
            else
            {
                gui->showMessage("Belum ada yang menginjaknya terlebih dahulu.");
                gui->showMessage(prop->getName() + " kini menjadi milikmu.");
            }
            if (price > 0)
            {
                gui->showMessage("Uang tersisa: " + Formatter::money(player->getBalance()));
            }
            if (logger)
            {
                const std::string actionType =
                    prop->getType() == PropertyType::RAILROAD ? "RAILROAD" : "UTILITAS";
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            actionType,
                            propertyLogLabel(prop) + " kini milik " + player->getUsername() +
                                (price > 0 ? " (otomatis, " + Formatter::money(price) + ")" : " (otomatis)"));
                gui->renderLog(logger->getFullLog(), "LOG");
            }
            return;
        }

        Player *owner = prop->getOwner();
        if (owner == player)
        {
            gui->showMessage("Kamu mendarat di propertimu sendiri: " + prop->getName() + ".");
            return;
        }
        if (prop->isMortgaged())
        {
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
        if (!condition.empty())
        {
            gui->showMessage("Kondisi      : " + condition);
        }
        gui->showMessage("Sewa         : " + Formatter::money(rent));
        int payerBefore = player->getBalance();
        int ownerBefore = owner->getBalance();
        bool paid = executePayment(player, owner, rent,
                                   "sewa " + Formatter::money(rent) +
                                       " kepada " + owner->getUsername());
        if (paid)
        {
            if (payerBefore >= rent)
            {
                gui->showMessage("Uang kamu     : " + Formatter::money(payerBefore) + " -> " + Formatter::money(player->getBalance()));
                gui->showMessage("Uang " + owner->getUsername() + " : " + Formatter::money(ownerBefore) + " -> " + Formatter::money(owner->getBalance()));
            }
            if (logger)
            {
                std::string detail = "Bayar " + Formatter::money(rent) + " ke " +
                                     owner->getUsername() + " (" + prop->getCode();
                if (!condition.empty())
                {
                    detail += ", " + condition;
                }
                detail += ")";
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "SEWA", detail);
                gui->renderLog(logger->getFullLog(), "LOG");
            }
        }
        return;
    }

    if (tile->getKind() == TileKind::INCOME_TAX)
    {
        auto *incomeTax = static_cast<IncomeTaxTile *>(tile);
        gui->showMessage("Kamu mendarat di Pajak Penghasilan (PPH).");
        gui->showMessage("1. Bayar flat " + Formatter::money(incomeTax->getFlatAmount()));
        gui->showMessage("2. Bayar " + std::to_string(incomeTax->getTaxPercentage()) +
                         "% dari total kekayaan (Pilih sebelum menghitung kekayaan!)");
        int choice = askIncomeTaxChoice(gui);
        int amount = 0;
        int before = player->getBalance();
        if (choice == 1)
        {
            amount = incomeTax->getFlatAmount();
            gui->showMessage("Kamu memilih membayar flat sebesar " + Formatter::money(amount) + ".");
        }
        else
        {
            int cash = player->getBalance();
            int propertyValue = player->calculatePropertyAssetValue();
            int buildingValue = player->calculateBuildingAssetValue();
            int wealth = cash + propertyValue + buildingValue;
            amount = (wealth * incomeTax->getTaxPercentage()) / 100;
            gui->showMessage("Total kekayaan kamu:");
            gui->showMessage("- Uang tunai          : " + Formatter::money(cash));
            gui->showMessage("- Harga beli properti : " + Formatter::money(propertyValue));
            gui->showMessage("- Harga beli bangunan : " + Formatter::money(buildingValue));
            gui->showMessage("Total                 : " + Formatter::money(wealth));
            gui->showMessage("Pajak " + std::to_string(incomeTax->getTaxPercentage()) +
                             "%             : " + Formatter::money(amount));
        }
        bool paid = executePayment(player, nullptr, amount,
                                   "Pajak Penghasilan " + Formatter::money(amount));
        if (paid)
        {
            if (before >= amount)
            {
                gui->showMessage("Uang kamu: " + Formatter::money(before) + " -> " + Formatter::money(player->getBalance()));
            }
            if (logger)
            {
                const std::string detail = (choice == 1)
                                               ? "PPH flat " + Formatter::money(amount)
                                               : "PPH " + std::to_string(incomeTax->getTaxPercentage()) +
                                                     "% = " + Formatter::money(amount);
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "PAJAK", detail);
                gui->renderLog(logger->getFullLog(), "LOG");
            }
        }
        return;
    }

    if (tile->getKind() == TileKind::LUXURY_TAX)
    {
        auto *luxuryTax = static_cast<LuxuryTaxTile *>(tile);
        int amount = luxuryTax->getFlatAmount();
        gui->showMessage("Kamu mendarat di Pajak Barang Mewah (PBM).");
        gui->showMessage("Pajak sebesar " + Formatter::money(amount) + " langsung dipotong.");
        int before = player->getBalance();
        bool paid = executePayment(player, nullptr, amount,
                                   "Pajak Barang Mewah " + Formatter::money(amount));
        if (paid)
        {
            if (before >= amount)
            {
                gui->showMessage("Uang kamu: " + Formatter::money(before) + " -> " + Formatter::money(player->getBalance()));
            }
            if (logger)
            {
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "PAJAK", "PBM " + Formatter::money(amount));
                gui->renderLog(logger->getFullLog(), "LOG");
            }
        }
        return;
    }

    if (tile->getKind() == TileKind::FESTIVAL)
    {
        if (player->getOwnedProperties().empty())
        {
            if (logger != nullptr)
            {
                logger->log(game->getCurrentTurn(), player->getUsername(),
                            "FESTIVAL", "Mendarat di Festival, tetapi tidak punya properti");
                gui->renderLog(logger->getFullLog(), "LOG");
            }
            gui->showMessage(player->getUsername() +
                             " mendarat di Festival, tetapi tidak memiliki properti.");
            gui->showMessage("Efek Festival hangus.");
            return;
        }
        player->setPendingFestival(true);
        if (logger != nullptr)
        {
            logger->log(game->getCurrentTurn(), player->getUsername(),
                        "FESTIVAL", "Mendarat di Festival dan harus memilih properti");
            gui->renderLog(logger->getFullLog(), "LOG");
        }
        gui->showMessage("Kamu mendarat di Festival.");
        gui->showMessage("Gunakan FESTIVAL <kode_properti> untuk memilih properti yang sewanya akan digandakan.");
        return;
    }

    if (tile->getKind() == TileKind::GO)
    {
        auto *goTile = static_cast<GoTile *>(tile);
        gui->showMessage(player->getUsername() + " mendarat tepat di petak GO.");
        gui->showMessage("Kamu menerima " + Formatter::money(goTile->getSalary()) + ".");
        return;
    }

    if (tile->getKind() == TileKind::GO_TO_JAIL)
    {
        player->setStatus(PlayerStatus::JAILED);
        if (game->getBoard() && game->getBoard()->getJailTile())
        {
            player->setPosition(game->getBoard()->getJailTile()->getIndex());
        }
        if (logger != nullptr)
        {
            logger->log(game->getCurrentTurn(), player->getUsername(),
                        "PENJARA", "Mendarat di Pergi ke Penjara -> masuk Penjara");
            gui->renderLog(logger->getFullLog(), "LOG");
        }
        gui->showMessage(player->getUsername() + " harus pergi ke Penjara.");
        return;
    }

    if (tile->getKind() == TileKind::JAIL)
    {
        if (player->isJailed())
        {
            gui->showMessage(player->getUsername() + " sedang berada di Penjara.");
        }
        else
        {
            gui->showMessage(player->getUsername() + " hanya mampir ke Penjara.");
        }
        return;
    }

    if (tile->getKind() == TileKind::FREE_PARKING)
    {
        gui->showMessage(player->getUsername() + " mendarat di Bebas Parkir.");
        return;
    }
}

bool GameEngine::executePayment(Player *from, Player *to, int amount,
                                const std::string &obligationLabel)
{
    if (from == nullptr || amount <= 0)
        return false;
    if (turnManager != nullptr && turnManager->isShieldActive())
    {
        gui->showMessage(from->getUsername() + " terlindungi oleh Perisai.");
        gui->showMessage("Kewajiban " + obligationLabel + " dibatalkan.");
        return true;
    }
    if (bankruptcyManager != nullptr)
    {

        bool result = bankruptcyManager->handleInsufficientFunds(*from, amount, to, obligationLabel);
        if (from->getStatus() == PlayerStatus::BANKRUPT)
        {
            gui->renderBankruptcy(*from);
        }
        return result;
    }
    if (!from->canAfford(amount))
        return false;
    from->deductMoney(amount);
    if (to != nullptr)
        to->addMoney(amount);
    return true;
}

bool GameEngine::checkWinCondition()
{
    if (game == nullptr)
        return false;
    if (game->getActivePlayerCount() <= 1)
        return true;
    if (game->isMaxTurnReached())
        return true;
    return false;
}

void GameEngine::endGame()
{
    if (game == nullptr || gui == nullptr)
        return;

    const auto &active = game->getActivePlayers();
    if (active.empty())
    {
        gui->loadFinishMenu();
        return;
    }

    // Multi-criteria: totalWealth desc → property count desc → card count desc → all tied = everyone wins
    int bestWealth = -1;
    for (Player *p : active)
        bestWealth = std::max(bestWealth, p->calculateTotalWealth());

    std::vector<Player *> candidates;
    for (Player *p : active)
        if (p->calculateTotalWealth() == bestWealth)
            candidates.push_back(p);

    if (candidates.size() > 1)
    {
        int bestProps = -1;
        for (Player *p : candidates)
            bestProps = std::max(bestProps, (int)p->getOwnedProperties().size());
        std::vector<Player *> next;
        for (Player *p : candidates)
            if ((int)p->getOwnedProperties().size() == bestProps)
                next.push_back(p);
        candidates = next;
    }

    if (candidates.size() > 1)
    {
        int bestCards = -1;
        for (Player *p : candidates)
            bestCards = std::max(bestCards, p->getCardCount());
        std::vector<Player *> next;
        for (Player *p : candidates)
            if (p->getCardCount() == bestCards)
                next.push_back(p);
        candidates = next;
    }

    gui->loadFinishMenu();
    if (candidates.size() == 1)
    {
        gui->renderWinner(*candidates[0]);
    }
    else
    {
        for (Player *p : candidates)
            gui->renderWinner(*p);
    }
}