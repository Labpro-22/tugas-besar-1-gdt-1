#include "models/BoardAndTiles/ActionTile/FestivalTile.hpp"
#include "core/Game.hpp"
#include "models/Player/Player.hpp"
#include <iostream>

FestivalTile::FestivalTile(int index)
    : ActionTile(index, "FES", "Festival") {}

void FestivalTile::onLanded(Player &player, Game & /*game*/)
{
    if (player.getOwnedProperties().empty()) {
        std::cout << player.getUsername()
                  << " mendarat di Festival, tapi tidak memiliki properti. Festival hangus.\n";
        return;
    }

    player.setPendingFestival(true);
    std::cout << player.getUsername()
              << " mendarat di Festival! Pilih properti untuk digandakan sewanya.\n"
              << "Ketik: FESTIVAL <kode_properti>\n";
}