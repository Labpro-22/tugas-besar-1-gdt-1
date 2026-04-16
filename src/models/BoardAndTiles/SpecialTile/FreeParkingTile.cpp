#include "models/BoardAndTiles/SpecialTile/FreeParkingTile.hpp"
// #include "core/Game.hpp"
// #include "models/Player/Player.hpp"

FreeParkingTile::FreeParkingTile(int index)
    : SpecialTile(index, "BBP", "Bebas Parkir") {}

void FreeParkingTile::onLanded(Player &player, Game &game)
{
    // no-op
}