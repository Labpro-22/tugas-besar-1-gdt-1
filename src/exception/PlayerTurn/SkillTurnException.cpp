#include "exception/PlayerTurn/SkillTurnException.hpp"

SkillTurnException::SkillTurnException(int code, const std::string &msg, Player *player, SkillCard *skillCard)
    : PlayerTurnException(code, msg, player), skillCard(skillCard) {};