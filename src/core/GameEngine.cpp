#include "core/GameEngine.hpp"

#include <iostream>

GameEngine::GameEngine(IGUI *gui)
    : game(nullptr),
      logger(new TransactionLogger()),
      gui(gui),
      turnManager(nullptr),
      commandProcessor(nullptr),
      auctionManager(nullptr),
      bankruptcyManager(nullptr),
      saveLoadManager(nullptr) {}

GameEngine::~GameEngine()
{
    // delete manager di sini setelah header-nya tersedia
    delete logger;
    delete game;
}

void GameEngine::update()
{
    if (gui == nullptr)
        return;

    gui->update();
    Command cmd = gui->getCommand();

    if (!cmd.isNull())
    {
        std::string path = cmd.getArgs().empty() ? "data/default" : cmd.getArgs()[0];

        if (cmd.getType() == "NEW_GAME")
        {
            initNewGame(path);
            gui->loadGameView();
            gui->renderBoard(*game);
        }
        else if (cmd.getType() == "LOAD_GAME")
        {
            initLoadGame(path);
            gui->loadGameView();
            gui->renderBoard(*game);
        }
    }
}

void GameEngine::initNewGame(const std::string &configPath)
{
    this->game = new Game();

    ConfigLoader loader(configPath);
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

    // TODO: tanya jumlah pemain, buat Player, acak turnOrder
}

void GameEngine::initLoadGame(const std::string &configPath)
{
    // TODO: load dari file via saveLoadManager
    initNewGame(configPath);
}

void GameEngine::gameLoop()
{
    // TODO: loop tiap giliran sampai game over
}

void GameEngine::processPlayerTurn(Player * /*player*/)
{
    // TODO: startTurn → loop perintah → endTurn
}

void GameEngine::handleTileLanding(Player * /*player*/, Tile * /*tile*/)
{
    // TODO: dispatch berdasarkan getCategory()
}

void GameEngine::handlePropertyLanding(Player * /*player*/, PropertyTile * /*tile*/)
{
    // TODO
}

void GameEngine::handleActionLanding(Player * /*player*/, ActionTile * /*tile*/)
{
    // TODO
}

void GameEngine::handleSpecialLanding(Player * /*player*/, SpecialTile * /*tile*/)
{
    // TODO
}

void GameEngine::handleStreetLanding(Player * /*player*/, StreetTile * /*tile*/)
{
    // TODO: beli / sewa / lelang
}

void GameEngine::handleRailroadLanding(Player * /*player*/, RailroadTile * /*tile*/)
{
    // TODO
}

void GameEngine::handleUtilityLanding(Player * /*player*/, UtilityTile * /*tile*/)
{
    // TODO
}

void GameEngine::handleChanceLanding(Player * /*player*/, ChanceTile * /*tile*/)
{
    // TODO: draw + execute kartu chance
}

void GameEngine::handleCommunityChestLanding(Player * /*player*/, CommunityChestTile * /*tile*/)
{
    // TODO
}

void GameEngine::handleFestivalLanding(Player * /*player*/, FestivalTile * /*tile*/)
{
    // TODO
}

void GameEngine::handleTaxLanding(Player * /*player*/, TaxTile * /*tile*/)
{
    // TODO
}

void GameEngine::handleGoToJailLanding(Player * /*player*/)
{
    // TODO
}

bool GameEngine::executePayment(Player * /*from*/, Player * /*to*/, int /*amount*/)
{
    // TODO: canAfford → bayar, kalau gagal → bankruptcyManager
    return false;
}

bool GameEngine::checkWinCondition()
{
    // TODO: satu pemain tersisa atau maxTurn tercapai
    return false;
}

void GameEngine::endGame()
{
    // TODO: tentukan pemenang + render
}

void GameEngine::executeGadai(Player * /*player*/)
{
    // TODO
}

void GameEngine::executeTebus(Player * /*player*/)
{
    // TODO
}

void GameEngine::executeBangun(Player * /*player*/)
{
    // TODO
}

void GameEngine::executeGunakanKemampuan(Player * /*player*/)
{
    // TODO
}
