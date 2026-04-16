#include "core/Game.hpp"

#include <cstdlib>
#include <stdexcept>

// DiceManager
DiceManager::DiceManager() : die1(0), die2(0) {}

std::pair<int, int> DiceManager::rollRandom() {
    die1 = rand() % 6 + 1;
    die2 = rand() % 6 + 1;
    return {die1, die2};
}

void DiceManager::setManual(int d1, int d2) {
    if (d1 < 1 || d1 > 6 || d2 < 1 || d2 > 6) {
        throw std::out_of_range("Nilai dadu harus pada rentang 1..6.");
    }
    die1 = d1;
    die2 = d2;
}

int DiceManager::getDie1() const { return die1; }
int DiceManager::getDie2() const { return die2; }
int DiceManager::getTotal() const { return die1 + die2; }
bool DiceManager::isDouble() const { return die1 == die2; }

// Game
Game::Game()
    : gameOver(false),
      lastDiceTotal(0),
      currentTurnIndex(0),
      currentTurn(1),
      maxTurn(0),
      initialBalance(0),
      goSalary(0),
      jailFine(0),
      pphFlat(0),
      pphPercent(0),
      pbmFlat(0),
      chanceDeck(nullptr),
      communityDeck(nullptr),
      skillDeck(nullptr),
      board(nullptr) {}

Game::~Game(){
    delete board;
    delete chanceDeck;
    delete communityDeck;
    delete skillDeck;
    for (Player* player : players) {
        delete player;
    }
    players.clear();
}

void Game::setBoard(Board* newBoard) {
    if (board != newBoard) {
        delete board;
        board = newBoard;
    }
}

void Game::setDecks(CardDeck<ChanceCard>* newChanceDeck, CardDeck<CommunityChestCard>* newCommunityDeck, CardDeck<SkillCard>* newSkillDeck) {
    if (chanceDeck != newChanceDeck) {
        delete chanceDeck;
        chanceDeck = newChanceDeck;
    }
    if (communityDeck != newCommunityDeck) {
        delete communityDeck;
        communityDeck = newCommunityDeck;
    }
    if (skillDeck != newSkillDeck) {
        delete skillDeck;
        skillDeck = newSkillDeck;
    }
}

void Game::addPlayer(Player* player) {
    if (player != nullptr) {
        players.push_back(player);
    }
}

void Game::setTurnOrder(const std::vector<int>& newTurnOrder) {
    turnOrder = newTurnOrder;
    setCurrentTurnIndex(currentTurnIndex);
}

void Game::setConfigValues(int newMaxTurn, int newInitialBalance, int newGoSalary, int newJailFine, int newPphFlat, int newPphPercent, int newPbmFlat, const std::map<int, int>& newRailroadRents, const std::map<int, int>& newUtilityMultipliers) {
    maxTurn = newMaxTurn;
    initialBalance = newInitialBalance;
    goSalary = newGoSalary;
    jailFine = newJailFine;
    pphFlat = newPphFlat;
    pphPercent = newPphPercent;
    pbmFlat = newPbmFlat;
    railroadRents = newRailroadRents;
    utilityMultipliers = newUtilityMultipliers;
}

void Game::setCurrentTurn(int newCurrentTurn) {
    currentTurn = (newCurrentTurn < 1) ? 1 : newCurrentTurn;
}

void Game::setCurrentTurnIndex(int index) {
    std::size_t containerSize = turnOrder.empty() ? players.size() : turnOrder.size();
    if (containerSize == 0) {
        currentTurnIndex = 0;
        return;
    }

    int normalizedIndex = index % static_cast<int>(containerSize);
    if (normalizedIndex < 0) {
        normalizedIndex += static_cast<int>(containerSize);
    }
    currentTurnIndex = normalizedIndex;
}

Player* Game::getCurrentPlayer() const {
    if (players.empty()) {
        return nullptr;
    }
    if (!turnOrder.empty() && currentTurnIndex < static_cast<int>(turnOrder.size())) {
        int playerIndex = turnOrder[currentTurnIndex];
        if (playerIndex >= 0 && playerIndex < static_cast<int>(players.size())) {
            return players[playerIndex];
        }
    }
    return players[currentTurnIndex % players.size()];
}

std::vector<Player*> Game::getActivePlayers() const {
    std::vector<Player*> activePlayers;
    for (Player* player : players) {
        if (player != nullptr && player->getStatus() != PlayerStatus::BANKRUPT) {
            activePlayers.push_back(player);
        }
    }
    return activePlayers;
}

int Game::getActivePlayerCount() const {
    return static_cast<int>(getActivePlayers().size());
}

Player* Game::getPlayer(std::size_t index) const {
    if (index >= players.size()) {
        return nullptr;
    }
    return players[index];
}

void Game::advanceTurnOrder() {
    std::size_t containerSize = turnOrder.empty() ? players.size() : turnOrder.size();
    if (containerSize == 0) {
        return;
    }

    currentTurnIndex = (currentTurnIndex + 1) % static_cast<int>(containerSize);
    if (currentTurnIndex == 0) {
        incrementTurn();
    }
}

void Game::incrementTurn() {
    currentTurn++;
}

bool Game::isMaxTurnReached() const {
    return maxTurn > 0 && currentTurn >= maxTurn;
}

Board* Game::getBoard() const {
    return board;
}

const std::vector<Player*>& Game::getPlayers() const {
    return players;
}

CardDeck<ChanceCard>* Game::getChanceDeck() const {
    return chanceDeck;
}

CardDeck<CommunityChestCard>* Game::getCommunityDeck() const {
    return communityDeck;
}

CardDeck<SkillCard>* Game::getSkillDeck() const {
    return skillDeck;
}

int Game::getLastDiceTotal() const {
    return lastDiceTotal;
}

void Game::setLastDiceTotal(int total) {
    lastDiceTotal = total;
}

int Game::getGoSalary() const {
    return goSalary;
}

int Game::getJailFine() const {
    return jailFine;
}

int Game::getPphFlat() const {
    return pphFlat;
}

int Game::getPphPercent() const {
    return pphPercent;
}

int Game::getPphPercentage() const {
    return getPphPercent();
}

int Game::getPbmFlat() const {
    return pbmFlat;
}

int Game::getInitialBalance() const {
    return initialBalance;
}

const std::map<int, int>& Game::getRailroadRents() const {
    return railroadRents;
}

const std::map<int, int>& Game::getRailroadRent() const {
    return getRailroadRents();
}

const std::map<int, int>& Game::getUtilityMultipliers() const {
    return utilityMultipliers;
}

const std::map<int, int>& Game::getUtilityMultiplier() const {
    return getUtilityMultipliers();
}

const std::vector<int>& Game::getTurnOrder() const {
    return turnOrder;
}

int Game::getCurrentTurnIndex() const {
    return currentTurnIndex;
}

bool Game::isGameOver() const {
    return gameOver;
}

void Game::setGameOver(bool gameOver) {
    this->gameOver = gameOver;
}

int Game::getCurrentTurn() const {
    return currentTurn;
}

int Game::getMaxTurn() const {
    return maxTurn;
}
