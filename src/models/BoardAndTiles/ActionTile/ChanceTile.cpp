#include "models/BoardAndTiles/ActionTile/ChanceTile.hpp"
// #include "models/Card/ActionCard.hpp"
// #include "core/Game.hpp"
// #include "models/Player/Player.hpp"

ChanceTile::ChanceTile(int index)
    : ActionTile(index, "KSP", "Kesempatan") {}

void ChanceTile::onLanded(Player &player, Game &game)
{
    // ActionCard *card = game.getChanceDeck().draw();

    // if (card == nullptr)
    //     return;

    // card->execute(player, game);
    // game.getChanceDeck().discard(card);
}