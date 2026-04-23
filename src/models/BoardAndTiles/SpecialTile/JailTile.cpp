#include "models/BoardAndTiles/SpecialTile/JailTile.hpp"
#include "models/Player/Player.hpp"

JailTile::JailTile(int index)
    : SpecialTile(index, "PEN", "Penjara") {}

void JailTile::onLanded(Player &player, Game &game)
{
    // no-op
}