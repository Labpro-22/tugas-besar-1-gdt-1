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

    if (cmd.isNull())
        return;

    cmd.debugPrint();

    if (cmd.getType() == "NEW_GAME")
    {
        std::string path = (cmd.getArgs().empty() || cmd.getArgs()[0].empty())
                               ? "data/config/default"
                               : cmd.getArgs()[0];
        initNewGame(path);
        waitingPlayerCount = true;
        gui->showInputPrompt("Jumlah pemain (2-4):");
        return;
    }
    else if (cmd.getType() == "LOAD_GAME")
    {
        if (cmd.getArgs().empty() || cmd.getArgs()[0].empty())
        {
            gui->showMessage("Path tidak boleh kosong");
            return;
        }

        std::string path = cmd.getArgs()[0];
        initLoadGame(path);
        gui->loadGameView();
        gui->renderBoard(*game);
    }
    else if (waitingPlayerCount && cmd.getType() == "INPUT")
    {
        if (cmd.getArgs().empty() || cmd.getArgs()[0].empty())
        {
            gui->showMessage("Masukkan angka 2-4");
            return;
        }

        const std::string &s = cmd.getArgs()[0];

        if (!std::all_of(s.begin(), s.end(), ::isdigit))
        {
            gui->showMessage("Input harus angka");
            return;
        }

        int count = std::stoi(s);

        if (count < 2 || count > 4)
        {
            gui->showMessage("Jumlah pemain harus 2-4");
            return;
        }

        setupPlayers(count);
        waitingPlayerCount = false;

        gui->loadGameView();
        gui->renderBoard(*game);
    }
    else if (cmd.getType() == "EXIT")
    {
        gui->requestExit();
    }
}

void GameEngine::setupPlayers(int count)
{
    if (game == nullptr) return;

    std::vector<int> order;
    order.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        std::string name = "Player " + std::to_string(i + 1);

        Player* p = new Player(name, game->getInitialBalance());

        p->setPosition(0);

        p->setStatus(PlayerStatus::ACTIVE);

        p->resetJailAttempts();
        p->resetConsecutiveDoubles();

        game->addPlayer(p);
        order.push_back(i);
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(order.begin(), order.end(), g);

    game->setTurnOrder(order);
    game->setCurrentTurnIndex(0);
    game->setCurrentTurn(1);
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
