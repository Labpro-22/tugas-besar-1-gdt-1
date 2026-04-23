#include "models/Player/Player.hpp"
#include "models/Property/Property.hpp"
#include "models/Property/StreetProperty.hpp"
#include "models/Property/RailroadProperty.hpp"
#include "models/Property/UtilityProperty.hpp"
#include "models/CardAndDeck/SkillCard.hpp"

Player::Player(const std::string &username, int initialBalance)
    : username(username),
      balance(initialBalance),
      position(0),
      status(PlayerStatus::ACTIVE),
      ownedProperties(),
      consecutiveDoubles(0),
      jailAttempts(0),
      hasRolledThisTurn(false),
      hasUsedSkillThisTurn(false)
{
}

// Getters
std::string Player::getUsername() const { return username; }
int Player::getBalance() const { return balance; }
int Player::getPosition() const { return position; }
PlayerStatus Player::getStatus() const { return status; }
int Player::getConsecutiveDoubles() const { return consecutiveDoubles; }
int Player::getJailAttempts() const { return jailAttempts; }

// Movement
int Player::move(int steps, int boardSize) {
    position = (position + steps) % boardSize;
    return position;
}

void Player::setPosition(int newPosition) {
    position = newPosition;
}

// Balance
void Player::addMoney(int amount) {
    balance += amount;
}

bool Player::deductMoney(int amount) {
    if (canAfford(amount)) {
        balance -= amount;
        return true;
    }
    return false;
}

bool Player::canAfford(int amount) const {
    return balance >= amount;
}

// Status
void Player::setStatus(PlayerStatus status) {
    this->status = status;
}

bool Player::isJailed() const {
    return status == PlayerStatus::JAILED;
}

void Player::incrementJailAttempts() {
    jailAttempts++;
}

void Player::resetJailAttempts() {
    jailAttempts = 0;
}

// Property Utils
int Player::countOwnedRailroads() const {
    int count = 0;
    for (Property *p : ownedProperties) {
        if (p->getType() == PropertyType::RAILROAD) count++;
    }
    return count;
}

int Player::countOwnedUtilities() const {
    int count = 0;
    for (Property *p : ownedProperties) {
        if (p->getType() == PropertyType::UTILITY) count++;
    }
    return count;
}

bool Player::ownsFullColorGroup(const std::string& colorGroup) const {
    // This requires knowledge of all properties in that group.
    // Simplifying: this logic usually resides in a Manager or requires a Board reference.
    // For now, return false or implement if we can find a way.
    return false; 
}

int Player::calculatePropertyAssetValue() const {
    int total = 0;
    for (Property *p : ownedProperties) {
        total += p->getPurchasePrice();
    }
    return total;
}

int Player::calculateBuildingAssetValue() const {
    int total = 0;
    for (Property *p : ownedProperties) {
        if (p->getType() == PropertyType::STREET) {
            StreetProperty *sp = static_cast<StreetProperty*>(p);
            total += (sp->getAssetValue() - sp->getPurchasePrice());
        }
    }
    return total;
}

int Player::calculateTotalWealth() const {
    return balance + calculatePropertyAssetValue() + calculateBuildingAssetValue();
}

// Turn State
void Player::startTurn() {
    hasRolledThisTurn = false;
    hasUsedSkillThisTurn = false;
}

void Player::markRolled() {
    hasRolledThisTurn = true;
}

void Player::markSkillUsed() {
    hasUsedSkillThisTurn = true;
}

bool Player::hasRolled() const {
    return hasRolledThisTurn;
}

bool Player::hasUsedSkill() const {
    return hasUsedSkillThisTurn;
}

// Skill Card
bool Player::addCard(SkillCard* card) {
    if (handCards.size() >= 3) {
        return false;
    }
    handCards.push_back(card);
    return true;
}

void Player::removeCard(SkillCard* card) {
    for (auto it = handCards.begin(); it != handCards.end(); ++it) {
        if (*it == card) {
            handCards.erase(it);
            return;
        }
    }
}

const std::vector<SkillCard*>& Player::getHandCards() const {
    return handCards;
}

int Player::getCardCount() const {
    return (int)handCards.size();
}

// Consecutive Doubles
void Player::incrementConsecutiveDoubles() {
    consecutiveDoubles++;
}

void Player::resetConsecutiveDoubles() {
    consecutiveDoubles = 0;
}

// Operator Overloading
Player& Player::operator+=(int amount) {
    addMoney(amount);
    return *this;
}

Player& Player::operator-=(int amount) {
    deductMoney(amount);
    return *this;
}

bool Player::operator>(const Player& other) const {
    return calculateTotalWealth() > other.calculateTotalWealth();
}

bool Player::operator<(const Player& other) const {
    return calculateTotalWealth() < other.calculateTotalWealth();
}