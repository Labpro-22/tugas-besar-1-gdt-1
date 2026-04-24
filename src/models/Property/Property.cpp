#include "models/Property/Property.hpp"

Property::Property(const std::string &code,
                   const std::string &name,
                   PropertyType type,
                   int purchasePrice,
                   int mortgageValue)
    : code(code),
      name(name),
      type(type),
      purchasePrice(purchasePrice),
      mortgageValue(mortgageValue),
      owner(nullptr),
      status(PropertyStatus::BANK),
      festivalMultiplier(1),
      festivalDuration(0)
{
}
std::string Property::getCode() const { return code; }
std::string Property::getName() const { return name; }
PropertyType Property::getType() const { return type; }
Player *Property::getOwner() const { return owner; }
PropertyStatus Property::getStatus() const { return status; }
int Property::getPurchasePrice() const { return purchasePrice; }
int Property::getMortgageValue() const { return mortgageValue; }
int Property::getFestivalMultiplier() const { return festivalMultiplier; }
int Property::getFestivalDuration() const { return festivalDuration; }
bool Property::isStreet() const { return type == PropertyType::STREET; }
bool Property::isRailroad() const { return type == PropertyType::RAILROAD; }
bool Property::isUtility() const { return type == PropertyType::UTILITY; }

void Property::setOwner(Player *owner) { this->owner = owner; }
void Property::clearOwner() { this->owner = nullptr; }
void Property::setStatus(PropertyStatus status) { this->status = status; }

bool Property::isOwned() const { return owner != nullptr; }
bool Property::isMortgaged() const { return status == PropertyStatus::MORTGAGED; }

void Property::activateFestival() {
    const int MAX_MULTIPLIER = 8;
    if (festivalDuration == 0 || festivalMultiplier < 2) {
        festivalMultiplier = 2;
    } else if (festivalMultiplier < MAX_MULTIPLIER) {
        festivalMultiplier *= 2;
        if (festivalMultiplier > MAX_MULTIPLIER) festivalMultiplier = MAX_MULTIPLIER;
    }
    festivalDuration = 3;
}

void Property::decrementFestivalDuration() {
    if (festivalDuration > 0) festivalDuration--;
    if (festivalDuration == 0) festivalMultiplier = 1;
}

void Property::setFestivalState(int multiplier, int duration) {
    festivalMultiplier = multiplier;
    festivalDuration = duration;
    if (festivalDuration <= 0) {
        festivalDuration = 0;
        festivalMultiplier = 1;
    }
    if (festivalMultiplier <= 0) {
        festivalMultiplier = 1;
    }
}
