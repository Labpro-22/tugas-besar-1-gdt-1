#ifndef TURNMANAGER_HPP
#define TURNMANAGER_HPP

#include <string>

class Game;
class Player;
class Tile;
class DiceManager;
class IGUI;
class TransactionLogger;
class SkillCard;

enum class TurnPhase {
    START,
    AWAITING_ROLL,
    POST_ROLL,
    ENDED
};

class TurnManager {
private:
    Game* game;
    DiceManager* dice;
    IGUI* gui;
    TransactionLogger* logger;

    TurnPhase phase;
    bool hasActed;
    bool shieldActive;

    void distributeSkillCard(Player* player);
    void handleDropCard(Player* player, SkillCard* drawnCard);
    void decrementFestivalDurations();
    std::string waitForInput(const std::string& prompt);
    

public:
    TurnManager(Game* game, DiceManager* dice, IGUI* gui, TransactionLogger* logger = nullptr);

    void startTurn(Player* player);
    void drawSkillCardForTurn(Player* player);
    void endTurn(Player* player);

    bool canRoll(Player* player) const;
    bool canUseSkill(Player* player) const;

    Tile* processMovement(Player* player, int steps);

    TurnPhase getPhase() const;
    void setPhase(TurnPhase p);

    bool isShieldActive() const;
    void setShieldActive(bool v);

    bool hasActedThisTurn() const;
    void markActed();
};

#endif
