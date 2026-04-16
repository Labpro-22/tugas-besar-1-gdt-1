#include "models/Player/Player.hpp"
#include "models/Property/Property.hpp"
#include "models/Property/StreetProperty.hpp"
#include "models/Property/RailroadProperty.hpp"
#include "models/Property/UtilityProperty.hpp"

Player::Player(const std::string &username, int initialBalance)
    : username(username),
      balance(initialBalance),
      position(0),
      status(PlayerStatus::ACTIVE),
      ownedProperties(),
      //   handCards(),
      consecutiveDoubles(0),
      jailAttempts(0),
      hasRolledThisTurn(false),
      hasUsedSkillThisTurn(false)
{
}