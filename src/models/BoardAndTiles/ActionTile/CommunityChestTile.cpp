#include "models/BoardAndTiles/ActionTile/CommunityChestTile.hpp"
// #include "models/Card/ActionCard.hpp"
// #include "core/Game.hpp"
// #include "models/Player/Player.hpp"

CommunityChestTile::CommunityChestTile(int index)
    : ActionTile(index, "DNU", "Dana Umum") {}

void CommunityChestTile::onLanded(Player &player, Game &game)
{
    // ActionCard *card = game.getCommunityChestDeck().draw();

    // if (card == nullptr)
    //     return;

    // card->execute(player, game);
    // game.getCommunityChestDeck().discard(card);
}