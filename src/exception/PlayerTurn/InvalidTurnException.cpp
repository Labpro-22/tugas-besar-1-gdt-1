#include "exception/PlayerTurn/InvalidTurnException.hpp"

InvalidTurnException::InvalidTurnException(Player *player)
    : PlayerTurnException(130, "Error Turn: ", player)
{
    errorMessage += "Nama Player: " + player->getUsername() +
                    " (Aksi ditolak karena saat ini bukan giliran player ini)";
}