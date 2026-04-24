#include "core/GameEngine.hpp"

#include <iostream>
#include <memory>

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
    if (!gui)
        return;

    gui->update();
    Command cmd = gui->getCommand();

    if (cmd.isNull())
        return;

    cmd.debugPrint();

    if (cmd.getType() == "EXIT")
    {
        gui->requestExit();
        currentHandler = nullptr;
        return;
    }

    if (cmd.getType() == "NEW_GAME")
    {
        currentHandler = nullptr;

        std::string path = (cmd.getArgs().empty() || cmd.getArgs()[0].empty())
                               ? "data/config/default"
                               : cmd.getArgs()[0];

        initNewGame(path);
        return;
    }

    if (cmd.getType() == "LOAD_GAME")
    {
        currentHandler = nullptr;

        if (cmd.getArgs().empty() || cmd.getArgs()[0].empty())
        {
            gui->showMessage("Path tidak boleh kosong");
            return;
        }

        std::string path = cmd.getArgs()[0];

        initLoadGame(path);
        gui->loadGameView();
        gui->renderBoard(*game);
        return;
    }

    if (currentHandler)
    {
        currentHandler(cmd);
        return;
    }
}

void GameEngine::startNameInputFlow(int count)
{
    auto names = std::make_shared<std::vector<std::string>>();
    auto index = std::make_shared<int>(0);
    auto handler = std::make_shared<std::function<void(const Command &)>>();

    *handler = [this, count, names, index, handler](const Command &cmd)
    {
        if (cmd.getType() != "INPUT")
            return;

        if (cmd.getArgs().empty() || cmd.getArgs()[0].empty())
        {
            gui->showMessage("Nama tidak boleh kosong");
            currentHandler = *handler;
            return;
        }

        names->push_back(cmd.getArgs()[0]);
        (*index)++;

        if (*index < count)
        {
            gui->showInputPrompt("Nama pemain " + std::to_string(*index + 1) + ":");
            currentHandler = *handler;
        }
        else
        {
            finalizePlayers(*names);
        }
    };

    gui->showInputPrompt("Nama pemain 1:");
    currentHandler = *handler;
}

void GameEngine::finalizePlayers(const std::vector<std::string> &names)
{
    std::vector<int> order;
    order.reserve(names.size());

    for (int i = 0; i < names.size(); ++i)
    {
        Player *p = new Player(names[i], game->getInitialBalance());

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

    currentHandler = nullptr;

    gui->loadGameView();
    gui->renderBoard(*game);

    for (auto p : game->getActivePlayers())
    {
        gui->loadPlayer(*p);
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

    gui->showInputPrompt("Jumlah pemain (2-4):");

    // currentHandler = [this](const Command &cmd)
    // {
    //     if (cmd.getType() != "INPUT")
    //     {
    //         return;
    //     }

    //     if (cmd.getArgs().empty() || cmd.getArgs()[0].empty())
    //     {
    //         gui->showMessage("Masukkan angka 2-4");
    //         return;
    //     }

    //     const std::string &s = cmd.getArgs()[0];

    //     if (!std::all_of(s.begin(), s.end(), ::isdigit))
    //     {
    //         gui->showMessage("Input harus angka");
    //         return;
    //     }

    //     int count = std::stoi(s);

    //     if (count < 2 || count > 4)
    //     {
    //         gui->showMessage("Jumlah pemain harus 2-4");
    //         return;
    //     }

    //     startNameInputFlow(count);
    // };
    currentHandler = [this](const Command &cmd)
    {
        std::vector<std::string> names = {"Player1", "Player2"};
        finalizePlayers(names);
    };
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
