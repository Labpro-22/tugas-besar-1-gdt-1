#include "core/GameEngine.hpp"

#include <iostream>

GameEngine::GameEngine(IGUI* gui)
    : game(nullptr),
      logger(new TransactionLogger()),
      gui(gui),
      turnManager(nullptr),
      commandProcessor(nullptr),
      auctionManager(nullptr),
      bankruptcyManager(nullptr),
      saveLoadManager(nullptr) {}

GameEngine::~GameEngine() {
    // delete manager di sini setelah header-nya tersedia
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

    // TODO: tanya jumlah pemain, buat Player, acak turnOrder
}

void GameEngine::initLoadGame() {
    // TODO: load dari file via saveLoadManager
}

void GameEngine::gameLoop() {
    // TODO: loop tiap giliran sampai game over
}

void GameEngine::processPlayerTurn(Player* /*player*/) {
    // TODO: startTurn → loop perintah → endTurn
}

void GameEngine::handleTileLanding(Player* /*player*/, Tile* /*tile*/) {
    // TODO: dispatch berdasarkan getCategory()
}

void GameEngine::handlePropertyLanding(Player* /*player*/, PropertyTile* /*tile*/) {
    // TODO
}

void GameEngine::handleActionLanding(Player* /*player*/, ActionTile* /*tile*/) {
    // TODO
}

void GameEngine::handleSpecialLanding(Player* /*player*/, SpecialTile* /*tile*/) {
    // TODO
}

void GameEngine::handleStreetLanding(Player* /*player*/, StreetTile* /*tile*/) {
    // TODO: beli / sewa / lelang
}

void GameEngine::handleRailroadLanding(Player* /*player*/, RailroadTile* /*tile*/) {
    // TODO
}

void GameEngine::handleUtilityLanding(Player* /*player*/, UtilityTile* /*tile*/) {
    // TODO
}

void GameEngine::handleChanceLanding(Player* /*player*/, ChanceTile* /*tile*/) {
    // TODO: draw + execute kartu chance
}

void GameEngine::handleCommunityChestLanding(Player* /*player*/, CommunityChestTile* /*tile*/) {
    // TODO
}

void GameEngine::handleFestivalLanding(Player* /*player*/, FestivalTile* /*tile*/) {
    // TODO
}

void GameEngine::handleTaxLanding(Player* /*player*/, TaxTile* /*tile*/) {
    // TODO
}

void GameEngine::handleGoToJailLanding(Player* /*player*/) {
    // TODO
}

bool GameEngine::executePayment(Player* /*from*/, Player* /*to*/, int /*amount*/) {
    // TODO: canAfford → bayar, kalau gagal → bankruptcyManager
    return false;
}

bool GameEngine::checkWinCondition() {
    // TODO: satu pemain tersisa atau maxTurn tercapai
    return false;
}

void GameEngine::endGame() {
    // TODO: tentukan pemenang + render
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
