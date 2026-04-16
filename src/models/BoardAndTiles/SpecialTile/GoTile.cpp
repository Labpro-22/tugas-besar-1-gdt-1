#include "models/BoardAndTiles/SpecialTile/GoTile.hpp"
// #include "core/Game.hpp"
// #include "models/Player/Player.hpp"

GoTile::GoTile(int index, int salary)
    : SpecialTile(index, "GO", "Petak Mulai"), salary(salary) {}

int GoTile::getSalary() const
{
    return salary;
}

// void GoTile::onLanded(Player &player, Game &game)
// {
//     player.getWallet().receive(salary);

//     game.getUI().display("Kamu berhenti tepat di Petak Mulai (GO)!");
//     game.getUI().display("Kamu menerima gaji M" + std::to_string(salary) + " dari Bank.");
//     game.getUI().display("Uang kamu: M" + std::to_string(player.getWallet().getBalance()));

//     game.getLogger().log(
//         game.getCurrentTurn(),
//         player.getUsername(),
//         "GAJI",
//         "Terima M" + std::to_string(salary) + " dari Bank (berhenti di GO)");
// }

// void GoTile::onPassed(Player &player, Game &game)
// {
//     player.getWallet().receive(salary);

//     game.getUI().display("Bidak " + player.getUsername() + " melewati Petak Mulai (GO).");
//     game.getUI().display("Kamu menerima gaji M" + std::to_string(salary) + " dari Bank.");
//     game.getUI().display("Uang kamu: M" + std::to_string(player.getWallet().getBalance()));

//     game.getLogger().log(
//         game.getCurrentTurn(),
//         player.getUsername(),
//         "GAJI",
//         "Terima M" + std::to_string(salary) + " dari Bank (melewati GO)");
// }