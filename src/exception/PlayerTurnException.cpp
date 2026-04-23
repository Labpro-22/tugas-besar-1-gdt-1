#include "exception/PlayerTurnException.hpp"

PlayerTurnException::PlayerTurnException(int code, const std::string &msg, Player *player)
    : GameException(code, msg), player(player) {}