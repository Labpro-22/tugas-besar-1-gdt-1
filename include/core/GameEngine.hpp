#ifndef GAMEENGINE_HPP
#define GAMEENGINE_HPP

#include "core/Game.hpp"
#include "core/TurnManager.hpp"
#include "utils/data/TransactionLogger.hpp"
#include "utils/data/ConfigLoader.hpp"
#include "views/IGUI.hpp"

class CommandProcessor;
class AuctionManager;
class BankruptcyManager;
class Property;
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

class GameEngine {
    friend class CommandProcessor;
private:
    Game* game;
    TransactionLogger* logger;
    IGUI* gui;  // non-owning

    DiceManager* dice;
    TurnManager* turnManager;
    CommandProcessor* commandProcessor;
    AuctionManager* auctionManager;
    BankruptcyManager* bankruptcyManager;
    SaveLoadManager* saveLoadManager;

    void initNewGame();
    void initLoadGame();

    void gameLoop();
    void processPlayerTurn(Player* player);

    void handleTileLanding(Player* player, Tile* tile);

    bool executePayment(Player* from, Player* to, int amount);
    bool checkWinCondition();
    void endGame();

public:
    GameEngine(IGUI* gui);
    ~GameEngine();

    void run();
};

#endif
