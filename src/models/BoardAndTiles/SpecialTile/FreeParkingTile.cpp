#include "models/BoardAndTiles/SpecialTile/FreeParkingTile.hpp"

FreeParkingTile::FreeParkingTile(int index)
    : SpecialTile(index, "BBP", "Bebas Parkir") {}

void FreeParkingTile::onLanded(Player &player, Game &game)
{
    // no-op
}