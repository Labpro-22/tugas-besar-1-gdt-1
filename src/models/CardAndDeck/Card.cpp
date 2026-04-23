#include "models/CardAndDeck/Card.hpp"

Card::Card(const std::string& description) 
    : description(description) {}

Card::~Card() {}

std::string Card::getDescription() const {
    return description;
}