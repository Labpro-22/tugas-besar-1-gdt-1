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