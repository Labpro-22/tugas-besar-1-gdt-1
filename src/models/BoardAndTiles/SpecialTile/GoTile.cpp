#include "models/BoardAndTiles/SpecialTile/GoTile.hpp"
#include "models/Player/Player.hpp"
#include <iostream>

GoTile::GoTile(int index, int salary)
    : SpecialTile(index, "GO", "Petak Mulai"), salary(salary) {}

int GoTile::getSalary() const
{
    return salary;
}

void GoTile::onLanded(Player &player, Game &game) {
    player.addMoney(salary);
    std::cout << player.getUsername() << " mendarat di petak GO!\n";
}

void GoTile::onPassed(Player &player, Game &game) {
    player.addMoney(salary);
    std::cout << player.getUsername() << " melewati petak GO!\n";
}