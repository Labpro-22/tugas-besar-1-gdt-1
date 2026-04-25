#include "models/Player/Player.hpp"
#include "models/Property/Property.hpp"
#include "models/Property/StreetProperty.hpp"
#include "models/Property/RailroadProperty.hpp"
#include "models/Property/UtilityProperty.hpp"
#include "models/CardAndDeck/SkillCard.hpp"

namespace {
int requiredStreetCount(const std::string& colorGroup) {
    if (colorGroup == "COKLAT" || colorGroup == "BIRU_TUA") {
        return 2;
    }

    if (colorGroup == "BIRU_MUDA" || colorGroup == "MERAH_MUDA" ||
        colorGroup == "ORANGE" || colorGroup == "MERAH" ||
        colorGroup == "KUNING" || colorGroup == "HIJAU") {
        return 3;
    }

    return 0;
}
}

Player::Player(const std::string &username, int initialBalance)
    : username(username),
      balance(initialBalance),
      position(0),
      status(PlayerStatus::ACTIVE),
      ownedProperties(),
      consecutiveDoubles(0),
      jailAttempts(0),
      hasRolledThisTurn(false),
      hasUsedSkillThisTurn(false),
      pendingFestival(false),
      pendingDiscount(0)
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

// Property ownership
void Player::addProperty(Property* property) {
    if (property == nullptr) return;
    ownedProperties.push_back(property);
}

void Player::removeProperty(Property* property) {
    for (auto it = ownedProperties.begin(); it != ownedProperties.end(); ++it) {
        if (*it == property) {
            ownedProperties.erase(it);
            return;
        }
    }
}

const std::vector<Property*>& Player::getOwnedProperties() const {
    return ownedProperties;
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
    const int required = requiredStreetCount(colorGroup);
    if (required <= 0) {
        return false;
    }

    int ownedCount = 0;
    for (Property *property : ownedProperties) {
        if (!property->isStreet()) continue;
        auto *street = static_cast<StreetProperty *>(property);
        if (street->getColorGroup() == colorGroup) {
            ++ownedCount;
        }
    }

    return ownedCount == required;
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
    pendingFestival = false;
    consecutiveDoubles = 0;
    pendingDiscount = 0;
}

bool Player::hasPendingFestival() const { return pendingFestival; }
void Player::setPendingFestival(bool v) { pendingFestival = v; }

int Player::getPendingDiscount() const { return pendingDiscount; }
void Player::setPendingDiscount(int pct) { pendingDiscount = pct; }
void Player::clearPendingDiscount() { pendingDiscount = 0; }

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

SkillCard* Player::findJailFreeCard() const {
    for (SkillCard* card : handCards) {
        if (card != nullptr && card->isJailFreeCard()) {
            return card;
        }
    }
    return nullptr;
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
