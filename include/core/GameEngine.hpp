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
    void handlePropertyLanding(Player* player, PropertyTile* tile);
    void handleActionLanding(Player* player, ActionTile* tile);
    void handleSpecialLanding(Player* player, SpecialTile* tile);
    void handleStreetLanding(Player* player, StreetTile* tile);
    void handleRailroadLanding(Player* player, RailroadTile* tile);
    void handleUtilityLanding(Player* player, UtilityTile* tile);
    void handleChanceLanding(Player* player, ChanceTile* tile);
    void handleCommunityChestLanding(Player* player, CommunityChestTile* tile);
    void handleFestivalLanding(Player* player, FestivalTile* tile);
    void handleTaxLanding(Player* player, TaxTile* tile);
    void handleGoToJailLanding(Player* player);

    bool executePayment(Player* from, Player* to, int amount);
    bool checkWinCondition();
    void endGame();

    void executeGadai(Player* player);
    void executeTebus(Player* player);
    void executeBangun(Player* player);
    void executeGunakanKemampuan(Player* player);

public:
    GameEngine(IGUI* gui);
    ~GameEngine();

    void run();
};

#endif
