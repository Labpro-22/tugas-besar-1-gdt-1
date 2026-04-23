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
enum class CommandResult : int;

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
    CommandResult handleJailedPlayerTurn(Player* player);
    CommandResult resolveRoll(Player* player, bool manual, int d1, int d2, bool fromJailAttempt);
    void handleChanceLanding(Player* player, ChanceTile* tile);
    void handleCommunityChestLanding(Player* player, CommunityChestTile* tile);

    void handleTileLanding(Player* player, Tile* tile);

    bool executePayment(Player* from, Player* to, int amount);
    bool checkWinCondition();
    void endGame();

    static std::string waitForInput(IGUI* gui, const std::string& prompt);
    static std::string normalizeInput(std::string s);
    static bool askYesNo(IGUI* gui, const std::string& prompt);
    static int askIncomeTaxChoice(IGUI* gui);

public:
    GameEngine(IGUI* gui);
    ~GameEngine();

    void run();
};

#endif
