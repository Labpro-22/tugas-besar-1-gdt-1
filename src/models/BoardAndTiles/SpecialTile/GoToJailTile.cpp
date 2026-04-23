#include "models/BoardAndTiles/SpecialTile/GoToJailTile.hpp"
#include "core/Game.hpp"
#include "models/Player/Player.hpp"
#include "models/BoardAndTiles/SpecialTile/JailTile.hpp"

GoToJailTile::GoToJailTile(int index)
    : SpecialTile(index, "PPJ", "Pergi ke Penjara") {}

void GoToJailTile::onLanded(Player &player, Game &game)
{
    player.setStatus(PlayerStatus::JAILED);
    if (game.getBoard() && game.getBoard()->getJailTile()) {
        player.setPosition(game.getBoard()->getJailTile()->getIndex());
    }
}