#include "Property.hpp"

Property::Property( const std::string& code,
                    const std::string& name,
                    PropertyType type,
                    int purchasePrice,
                    int mortgageValue)
    :   code(code),
        name(name),
        type(type),
        purchasePrice(purchasePrice),
        mortgageValue(mortgageValue),
        owner(nullptr),
        status(PropertyStatus::BANK),
        festivalMultiplier(1),
        festivalDuration(0) {
}

std::string Property::getCode() const {
    return code;
}

std::string Property::getName() const {
    return name;
}

PropertyType Property::getType() const {
    return type;
}

Player* Property::getOwner() const {
    return owner;
}

PropertyStatus Property::getStatus() const {
    return status;
}

int Property::getPurchasePrice() const {
    return purchasePrice;
}

int Property::getMortgageValue() const {
    return mortgageValue;
}

int Property::getFestivalMultiplier() const {
    return festivalMultiplier;
}

int Property::getFestivalDuration() const {
    return festivalDuration;
}

void Property::setOwner(Player* owner) {
    this->owner = owner;
    if (owner == nullptr) {
        status = PropertyStatus::BANK;
    } else if (status == PropertyStatus::BANK) {
        status = PropertyStatus::OWNED;
    }
}

void Property::clearOwner() {
    owner = nullptr;
    status = PropertyStatus::BANK;
    festivalMultiplier = 1;
    festivalDuration = 0;
}

void Property::setStatus(PropertyStatus status) {
    this->status = status;
}

bool Property::isOwned() const {
    return status == PropertyStatus::OWNED;
}

bool Property::isMortgaged() const {
    return status == PropertyStatus::MORTGAGED;
}

void Property::activateFestival() {
    if (festivalMultiplier < 8) {
        festivalMultiplier *= 2;
    }
    festivalDuration = 3;
}

void Property::decrementFestivalDuration() {
    if (festivalDuration > 0) {
        --festivalDuration;
        if (festivalDuration == 0) {
            festivalMultiplier = 1;
        }
    }
}

int Property::getBuildingAssetValue() const {
    return 0;
}