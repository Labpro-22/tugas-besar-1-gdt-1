#ifndef GAMEENGINE_HPP
#define GAMEENGINE_HPP

#include "Game.hpp"
#include "../utils/data/TransactionLogger.hpp"
#include "../../include/utils/data/ConfigLoader.hpp" 

class TurnManager;
class DiceManager;
class CommandProcessor;
class AuctionManager;
class BankruptcyManager;
class SaveLoadManager;

class GameEngine {
private:
    Game* game;
    TransactionLogger* logger;
    
    // IGUI* gui; 

    TurnManager* turnManager;
    DiceManager* diceManager;
    CommandProcessor* commandProcessor;
    AuctionManager* auctionManager;
    BankruptcyManager* bankruptcyManager;
    SaveLoadManager* saveLoadManager;

    void initNewGame();
    void initLoadGame();
    void gameLoop();
    void processPlayerTurn(Player* player);
    void handleTileLanding(Player* player, Tile* tile);

public:
    GameEngine(); 
    ~GameEngine();

    void run();
};

#endif