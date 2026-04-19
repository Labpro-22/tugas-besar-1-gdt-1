#include "core/GameEngine.hpp"

#include <iostream>
#include <algorithm>

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
#include "models/BoardAndTiles/SpecialTile/GoToJailTile.hpp"
#include "models/BoardAndTiles/SpecialTile/JailTile.hpp"

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
    delete turnManager;
    delete dice;
    delete logger;
    delete game;
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

    ConfigLoader loader("data/");
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

    // TODO: tanya jumlah pemain, buat Player, acak turnOrder
}

void GameEngine::initLoadGame() {
    // TODO: load dari file via saveLoadManager
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
    // TODO: loop perintah dari gui/commandProcessor sampai player END_TURN
    turnManager->endTurn(player);
}

void GameEngine::handleTileLanding(Player* player, Tile* tile) {
    if (player == nullptr || tile == nullptr || game == nullptr) return;

    switch (tile->getCategory()) {
        case TileCategory::PROPERTY:
            handlePropertyLanding(player, dynamic_cast<PropertyTile*>(tile));
            break;
        case TileCategory::ACTION:
            handleActionLanding(player, dynamic_cast<ActionTile*>(tile));
            break;
        case TileCategory::SPECIAL:
            handleSpecialLanding(player, dynamic_cast<SpecialTile*>(tile));
            break;
    }
}

void GameEngine::handlePropertyLanding(Player* player, PropertyTile* tile) {
    if (tile == nullptr) return;
    if      (auto* s = dynamic_cast<StreetTile*>(tile))   handleStreetLanding(player, s);
    else if (auto* r = dynamic_cast<RailroadTile*>(tile)) handleRailroadLanding(player, r);
    else if (auto* u = dynamic_cast<UtilityTile*>(tile))  handleUtilityLanding(player, u);
}

void GameEngine::handleActionLanding(Player* player, ActionTile* tile) {
    if (tile == nullptr) return;
    if      (auto* c = dynamic_cast<ChanceTile*>(tile))          handleChanceLanding(player, c);
    else if (auto* cc = dynamic_cast<CommunityChestTile*>(tile)) handleCommunityChestLanding(player, cc);
    else if (auto* f = dynamic_cast<FestivalTile*>(tile))        handleFestivalLanding(player, f);
    else if (auto* t = dynamic_cast<TaxTile*>(tile))             handleTaxLanding(player, t);
}

void GameEngine::handleSpecialLanding(Player* player, SpecialTile* tile) {
    if (tile == nullptr) return;
    if (dynamic_cast<GoToJailTile*>(tile)) {
        handleGoToJailLanding(player);
        return;
    }
    // GO / Jail / FreeParking delegasi ke tile.onLanded
    tile->onLanded(*player, *game);
}

void GameEngine::handleStreetLanding(Player* player, StreetTile* tile) {
    // TODO: beli / sewa / lelang via CommandProcessor
    tile->onLanded(*player, *game);
}

void GameEngine::handleRailroadLanding(Player* player, RailroadTile* tile) {
    tile->onLanded(*player, *game);
}

void GameEngine::handleUtilityLanding(Player* player, UtilityTile* tile) {
    tile->onLanded(*player, *game);
}

void GameEngine::handleChanceLanding(Player* player, ChanceTile* tile) {
    tile->onLanded(*player, *game);
}

void GameEngine::handleCommunityChestLanding(Player* player, CommunityChestTile* tile) {
    tile->onLanded(*player, *game);
}

void GameEngine::handleFestivalLanding(Player* player, FestivalTile* tile) {
    tile->onLanded(*player, *game);
}

void GameEngine::handleTaxLanding(Player* player, TaxTile* tile) {
    tile->onLanded(*player, *game);
}

void GameEngine::handleGoToJailLanding(Player* player) {
    Board* board = game->getBoard();
    if (board == nullptr) return;
    JailTile* jail = board->getJailTile();
    if (jail != nullptr) {
        player->setPosition(jail->getIndex());
    }
    player->setStatus(PlayerStatus::JAILED);
}

bool GameEngine::executePayment(Player* from, Player* to, int amount) {
    if (from == nullptr || amount <= 0) return false;
    if (!from->canAfford(amount)) {
        // TODO: trigger bankruptcyManager saat tersedia
        return false;
    }
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

void GameEngine::executeGadai(Player* /*player*/) {
    // TODO
}

void GameEngine::executeTebus(Player* /*player*/) {
    // TODO
}

void GameEngine::executeBangun(Player* /*player*/) {
    // TODO
}

void GameEngine::executeGunakanKemampuan(Player* /*player*/) {
    // TODO
}
