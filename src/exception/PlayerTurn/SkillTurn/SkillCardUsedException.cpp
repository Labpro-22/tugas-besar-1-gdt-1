#include "exception/PlayerTurn/SkillTurn/SkillCardUsedException.hpp"

SkillCardUsedException::SkillCardUsedException(Player *player, SkillCard *skillCard)
    : SkillTurnException(142, "Skill Card Used: ", player, skillCard)
{
    errorMessage += "Nama Player: " + player->getUsername() +
                    ", Skill Card: " + skillCard->getCardName() +
                    " (Kartu skill ini sudah digunakan dan tidak bisa dipakai lagi)";
}