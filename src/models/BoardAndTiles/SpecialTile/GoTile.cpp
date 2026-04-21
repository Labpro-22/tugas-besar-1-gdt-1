#include "models/BoardAndTiles/SpecialTile/GoTile.hpp"
#include "models/Player/Player.hpp"

GoTile::GoTile(int index, int salary)
    : SpecialTile(index, "GO", "Petak Mulai"), salary(salary) {}

int GoTile::getSalary() const
{
    return salary;
}

void GoTile::onLanded(Player &player, Game &game)
{
    player.addMoney(salary);
}

void GoTile::onPassed(Player &player, Game &game)
{
    player.addMoney(salary);
}