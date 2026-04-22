#include "exception/PlayerTurn/SkillTurn/DiceAlreadyRolledException.hpp"

DiceAlreadyRolledException::DiceAlreadyRolledException(Player *player, SkillCard *skillCard)
    : SkillTurnException(141, "Dice Already Rolled: ", player, skillCard)
{
    errorMessage += "Nama Player: " + player->getUsername() +
                    ", Skill Card: " + skillCard->getCardName() +
                    " (Gagal digunakan karena player sudah mengocok dadu pada giliran ini)";
}