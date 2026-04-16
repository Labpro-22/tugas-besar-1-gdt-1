#include "exception/PlayerTurn/PropertyManagementException.hpp"

PropertyManagementException::PropertyManagementException(int code, const std::string &msg, Player *player, Property *property)
    : PlayerTurnException(code, msg, player), property(property) {};