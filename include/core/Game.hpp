#ifndef GAME_HPP
#define GAME_HPP
#include <string>
#include <vector>
#include <map>
#include <cstddef>
#include <utility>
#include "models/Player/Player.hpp"
#include "models/BoardAndTiles/Board.hpp"
#include "models/CardAndDeck/CardDeck.hpp"
#include "models/CardAndDeck/ChanceCard.hpp"
#include "models/CardAndDeck/CommunityChestCard.hpp"
#include "models/CardAndDeck/SkillCard.hpp"

class DiceManager{
    private :
        int die1;
        int die2;
    public :
        DiceManager();
        std::pair<int, int> rollRandom();
        void setManual(int d1, int d2);
        int getDie1() const;
        int getDie2() const;
        int getTotal() const;
        bool isDouble() const;
};

class Game {
    private :
    bool gameOver;
    int lastDiceTotal;
    int currentTurnIndex;
    int currentTurn;
    int maxTurn;
    int initialBalance;
    int goSalary;
    int jailFine;
    int pphFlat;
    int pphPercent;
    int pbmFlat;
    std::map<int,int> railroadRents;
    std::map<int,int> utilityMultipliers;
    std::vector<int> turnOrder;
    std::vector<Player*> players;
    CardDeck<ChanceCard>* chanceDeck;
    CardDeck<CommunityChestCard>* communityDeck;
    CardDeck<SkillCard>* skillDeck;
    Board* board;
    public :
    Game();
    ~Game();

    // Setup API (digunakan engine saat init new/load game)
    void setBoard(Board* board);
    void setDecks(CardDeck<ChanceCard>* chanceDeck,CardDeck<CommunityChestCard>* communityDeck,CardDeck<SkillCard>* skillDeck);
    void addPlayer(Player* player);
    void setTurnOrder(const std::vector<int>& turnOrder);
    void setConfigValues(int maxTurn,int initialBalance,int goSalary,int jailFine,int pphFlat,int pphPercent,int pbmFlat,const std::map<int, int>& railroadRents,const std::map<int, int>& utilityMultipliers);
    void setCurrentTurn(int currentTurn);
    void setCurrentTurnIndex(int index);

    Player* getCurrentPlayer() const;
    std::vector<Player*> getActivePlayers() const;
    int getActivePlayerCount() const;
    Player* getPlayer(std::size_t index) const;
    void advanceTurnOrder(); // ururtan pemain dalam permainan
    void incrementTurn(); // Ronde berikutnya
    bool isMaxTurnReached() const;
    Board* getBoard() const;
    const std::vector<Player*>& getPlayers() const;
    CardDeck<ChanceCard>* getChanceDeck() const;
    CardDeck<CommunityChestCard>* getCommunityDeck() const;
    CardDeck<SkillCard>* getSkillDeck() const;
    int getLastDiceTotal() const;
    void setLastDiceTotal(int total);
    int getGoSalary() const;
    int getJailFine() const;
    int getPphFlat() const;
    int getPphPercent() const;
    int getPphPercentage() const;
    int getPbmFlat() const;
    int getInitialBalance() const;
    const std::map<int, int>& getRailroadRents() const;
    const std::map<int, int>& getRailroadRent() const;
    const std::map<int, int>& getUtilityMultipliers() const;
    const std::map<int, int>& getUtilityMultiplier() const;
    const std::vector<int>& getTurnOrder() const;
    int getCurrentTurnIndex() const;
    bool isGameOver() const;
    void setGameOver(bool gameOver);
    int getCurrentTurn() const;
    int getMaxTurn() const;
};

#endif
