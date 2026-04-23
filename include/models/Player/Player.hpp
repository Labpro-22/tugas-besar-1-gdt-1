#pragma once

#include <string>
#include <vector>

#include "../CardAndDeck/SkillCard.hpp"

class Property;

enum class PlayerStatus {
    ACTIVE,
    JAILED,
    BANKRUPT
};

class Player {
private:
    std::string username;
    int balance;
    int position;
    PlayerStatus status;

    std::vector<Property*> ownedProperties;
    std::vector<SkillCard*> handCards;

    int consecutiveDoubles;
    int jailAttempts;
    bool hasRolledThisTurn;
    bool hasUsedSkillThisTurn;
    bool pendingFestival;

public:
    //constructor
    Player(const std::string& username, int initialBalance);

    //getter
    std::string getUsername() const;
    int getBalance() const;
    int getPosition() const;
    PlayerStatus getStatus() const;
    int getConsecutiveDoubles() const; //tambahan
    int getJailAttempts() const; //tambahan

    //movement
    int move(int steps, int boardSize = 40);
    void setPosition(int newPosition);

    //balance
    void addMoney(int amount);
    bool deductMoney(int amount);
    bool canAfford(int amount) const;

    //status
    void setStatus(PlayerStatus status);
    bool isJailed() const;
    void incrementJailAttempts();
    void resetJailAttempts();

    //property -----------------------
    void addProperty(Property* property);
    void removeProperty(Property* property);
    const std::vector<Property*>& getOwnedProperties() const;

    //property util
    int countOwnedRailroads() const;
    int countOwnedUtilities() const;
    bool ownsFullColorGroup(const std::string& colorGroup) const;
    int calculatePropertyAssetValue() const;
    int calculateBuildingAssetValue() const;
    int calculateTotalWealth() const;

    //turn state
    void startTurn();
    void markRolled();
    void markSkillUsed();
    bool hasRolled() const;
    bool hasUsedSkill() const;
    bool hasPendingFestival() const;
    void setPendingFestival(bool v);

    //skill card---------------------------
    bool addCard(SkillCard* card);
    void removeCard(SkillCard* card);
    SkillCard* findJailFreeCard() const;
    const std::vector<SkillCard*>& getHandCards() const;
    int getCardCount() const;

    //consecutiveDouble
    void incrementConsecutiveDoubles(); //tambahan
    void resetConsecutiveDoubles(); //tambahan
    
    //operator overloading
    Player& operator+=(int amount);
    Player& operator-=(int amount);
    bool operator>(const Player& other) const;
    bool operator<(const Player& other) const;
};
