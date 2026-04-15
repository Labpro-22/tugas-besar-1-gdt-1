#include "Player.hpp"
#include "../Property/Property.hpp"
#include "../Property/StreetProperty.hpp"
#include "../Property/RailroadProperty.hpp"
#include "../Property/UtilityProperty.hpp"

Player::Player(const std::string& username, int initialBalance)
    :   username(username),
        balance(initialBalance),
        position(0),
        status(PlayerStatus::ACTIVE),
        ownedProperties(),
    //   handCards(),
        consecutiveDoubles(0),
        jailAttempts(0),
        hasRolledThisTurn(false),
        hasUsedSkillThisTurn(false) {
}

//getter
std::string Player::getUsername() const{
    return username;
}
int Player::getBalance() const{
    return balance;
}
int Player::getPosition() const{
    return position;
}
PlayerStatus Player::getStatus() const{
    return status;
}
int Player::getConsecutiveDoubles() const{
    return consecutiveDoubles;
}
int Player::getJailAttempts() const{
    return jailAttempts;
}

//movement
void Player::move(int steps, int boardSize = 40){
    setPosition((position + steps) % 40);
}
void Player::setPosition(int newPosition){
    position = newPosition;
}

//balance
void Player::addMoney(int amount){
    balance += amount;
}
bool Player::deductMoney(int amount){
    if (balance < amount){
        return false;
    }
    if (amount < 0){
        return false;
    }
    return balance -= amount;
}
bool Player::canAfford(int amount) const{
    return balance >= amount;
}

//status
void Player::setStatus(PlayerStatus status){
    this->status = status;
}
bool Player::isJailed() const{
    return status == PlayerStatus::JAILED;
}
void Player::incrementJailAttempts(){
    jailAttempts++;
}
void Player::resetJailAttempts(){
    jailAttempts = 0;
}

//jail
void Player::sendToJail(int jailIndex){
    position = jailIndex;
    status = PlayerStatus::JAILED;
    jailAttempts = 0;
    consecutiveDoubles = 0;
}
void Player::releaseFromJail(){
    status = PlayerStatus::ACTIVE;
    jailAttempts = 0;
    consecutiveDoubles = 0;
}

//property -----------------------
void Player::addProperty(Property* property){
    if (property == nullptr) {
        return;
    }

    ownedProperties.push_back(property);
}
void Player::removeProperty(Property* property){
    auto it = std::find(ownedProperties.begin(), ownedProperties.end(), property);
    if (it != ownedProperties.end()) {
        ownedProperties.erase(it);
    }
}
const std::vector<Property*>& Player::getOwnedProperties() const{
    return ownedProperties;
}

//property util
int Player::countOwnedRailroads() const{
    int count = 0;

    for (Property* property : ownedProperties){
        if (property != nullptr && property->getType() == PropertyType::RAILROAD){
            count++;
        }
    }
    return count;   
}
int Player::countOwnedUtilities() const{
    int count = 0;

    for (Property* property : ownedProperties){
        if (property != nullptr && property->getType() == PropertyType::UTILITY){
            count++;
        }
    }
    return count;   
}
bool Player::ownsFullColorGroup(const std::string& colorGroup) const {
    //SKIP DULU, DINAMIS
}
int Player::calculatePropertyAssetValue() const{
    int total = 0;

    for (Property* property : ownedProperties) {
        if (property != nullptr) {
            total += property->getAssetValue();
        }
    }

    return total;
}
int Player::calculateBuildingAssetValue() const{
    int total = 0;

    for (Property* property : ownedProperties) {
        if (property != nullptr) {
            total += property->getBuildingAssetValue();
        }
    }

    return total;
}
int Player::calculateTotalWealth() const{
    return balance + calculatePropertyAssetValue() + calculateBuildingAssetValue();
}

//turn state
void Player::startTurn(){
    hasRolledThisTurn = false;
    hasUsedSkillThisTurn = false;
    consecutiveDoubles = 0;
}
void Player::markRolled(){
    hasRolledThisTurn = true;
}
void Player::markSkillUsed(){
    hasUsedSkillThisTurn = true;
}
bool Player::hasRolled() const{
    return hasRolledThisTurn;
}
bool Player::hasUsedSkill() const{
    return hasUsedSkillThisTurn;
}

//skill card---------------------------
// bool Player::addCard(SkillCard* card);
// void Player::removeCard(SkillCard* card);
// const std::vector<SkillCard*>& Player::getHandCards() const;
// int Player::getCardCount() const;

//consecutiveDouble
void Player::incrementConsecutiveDoubles(){
    consecutiveDoubles++;
}
void Player::resetConsecutiveDoubles(){
    consecutiveDoubles = 0;
}

//operator overloading
Player& Player::operator+=(int amount){
    addMoney(amount);
    return *this;
}
Player& Player::operator-=(int amount){
    if (amount >= 0 && balance >= amount) {
        balance -= amount;
    }
    return *this;
}
bool Player::operator>(const Player& other) const{
    return calculateTotalWealth() > other.calculateTotalWealth();
}
bool Player::operator<(const Player& other) const{
    return calculateTotalWealth() < other.calculateTotalWealth();
}