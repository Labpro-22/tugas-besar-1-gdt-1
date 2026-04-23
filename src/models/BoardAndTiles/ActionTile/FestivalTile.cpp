#include "models/BoardAndTiles/ActionTile/FestivalTile.hpp"
// #include "core/Game.hpp"
#include "models/Player/Player.hpp"
#include "models/Property/Property.hpp"

FestivalTile::FestivalTile(int index)
    : ActionTile(index, "FES", "Festival") {}

void FestivalTile::onLanded(Player &player, Game &game)
{
    // const std::vector<Property *> &properties = player.getProperties();

    // if (properties.empty())
    // {
    //     game.getUI().display("Kamu tidak memiliki properti untuk dikenakan efek festival.");
    //     return;
    // }

    // game.getUI().displayPlayerProperties(player);
    // game.getUI().display("Masukkan kode properti: ");

    // std::string code = game.getUI().getInput();
    // Property *target = player.getPropertyByCode(code);

    // if (target == nullptr)
    // {
    //     game.getUI().display("Kode properti tidak valid atau bukan milikmu!");
    //     return;
    // }

    // target->applyFestival();

    // game.getLogger().log(
    //     game.getCurrentTurn(),
    //     player.getUsername(),
    //     "FESTIVAL",
    //     target->getCode() + ": sewa " +
    //         std::to_string(target->getCurrentRent() / target->getFestivalMultiplier()) +
    //         " -> " + std::to_string(target->getCurrentRent()) +
    //         ", durasi " + std::to_string(target->getFestivalDuration()) + " giliran");
}