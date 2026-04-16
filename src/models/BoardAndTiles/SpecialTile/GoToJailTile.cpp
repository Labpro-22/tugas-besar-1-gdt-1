#include "models/BoardAndTiles/SpecialTile/GoToJailTile.hpp"
// #include "core/Game.hpp"
#include "models/Player/Player.hpp"

GoToJailTile::GoToJailTile(int index)
    : SpecialTile(index, "PPJ", "Pergi ke Penjara") {}

// void GoToJailTile::onLanded(Player &player, Game &game)
// {
//     game.getUI().display("Kamu mendarat di Petak Pergi ke Penjara!");
//     game.getUI().display("Bidak kamu dipindahkan ke Penjara. Kamu sekarang berstatus tahanan.");

//     player.setStatus(PlayerStatus::JAILED);
//     player.setPosition(game.getBoard().getJailTile()->getIndex());
//     player.resetJailTurnCount();

//     game.getLogger().log(
//         game.getCurrentTurn(),
//         player.getUsername(),
//         "PENJARA",
//         player.getUsername() + " masuk penjara (mendarat di Pergi ke Penjara)");

//     game.getTurnManager().endTurn();
// }