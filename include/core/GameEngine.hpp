#ifndef GAMEENGINE_HPP
#define GAMEENGINE_HPP

#include "core/Game.hpp"
#include "utils/data/TransactionLogger.hpp"
#include "utils/data/ConfigLoader.hpp"
#include "views/IGUI.hpp"
#include <functional>

class TurnManager;
class CommandProcessor;
class AuctionManager;
class BankruptcyManager;
class SaveLoadManager;

class PropertyTile;
class ActionTile;
class SpecialTile;
class StreetTile;
class RailroadTile;
class UtilityTile;
class ChanceTile;
class CommunityChestTile;
class FestivalTile;
class TaxTile;

class GameEngine
{
private:
    Game *game;
    TransactionLogger *logger;
    IGUI *gui; // non-owning

    std::function<void(const Command &)> currentHandler;

    TurnManager *turnManager;
    CommandProcessor *commandProcessor;
    AuctionManager *auctionManager;
    BankruptcyManager *bankruptcyManager;
    SaveLoadManager *saveLoadManager;

    void initNewGame(const std::string &configPath);
    void initLoadGame(const std::string &configPath);

    void gameLoop();
    void processPlayerTurn(Player *player);
    void startNameInputFlow(int count);
    void finalizePlayers(const std::vector<std::string> &names);

    void handleTileLanding(Player *player, Tile *tile);

    bool executePayment(Player *from, Player *to, int amount);
    bool checkWinCondition();
    void endGame();

    void executeGadai(Player *player);
    void executeTebus(Player *player);
    void executeBangun(Player *player);
    void executeGunakanKemampuan(Player *player);

public:
    GameEngine(IGUI *gui);
    ~GameEngine();

    void update();
};

#endif
