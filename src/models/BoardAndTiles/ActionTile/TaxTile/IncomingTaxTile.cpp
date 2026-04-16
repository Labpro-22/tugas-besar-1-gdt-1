#include "models/BoardAndTiles/ActionTile/TaxTile/IncomingTaxTile.hpp"
// #include "core/Game.hpp"
#include "models/Player/Player.hpp"
#include "exception/PlayerTurn/PropertyManagement/InsufficientMoneyException.hpp"

IncomeTaxTile::IncomeTaxTile(int index, int flatAmount, int percentage)
    : TaxTile(index, "PPH", "Pajak Penghasilan"),
      flatAmount(flatAmount),
      percentage(percentage) {}

int IncomeTaxTile::calculateTax(const Player &player) const
{
    // int totalWealth = player.getWallet().getBalance() + player.getTotalPropertyValue() + player.getTotalBuildingValue();

    // return totalWealth * percentage / 100;
}

void IncomeTaxTile::onLanded(Player &player, Game &game)
{
    // game.getUI().display("Kamu mendarat di Pajak Penghasilan (PPH)!");
    // game.getUI().display("Pilih opsi pembayaran pajak:");
    // game.getUI().display("1. Bayar flat M" + std::to_string(flatAmount));
    // game.getUI().display("2. Bayar " + std::to_string(percentage) + "% dari total kekayaan");
    // game.getUI().display("(Pilih sebelum menghitung kekayaan!)");
    // game.getUI().display("Pilihan (1/2): ");

    // std::string choice = game.getUI().getInput();

    // if (choice == "1")
    // {
    //     try
    //     {
    //         player.getWallet().deduct(flatAmount);

    //         game.getLogger().log(
    //             game.getCurrentTurn(),
    //             player.getUsername(),
    //             "PAJAK",
    //             "Bayar PPH flat M" + std::to_string(flatAmount) + " ke Bank");
    //     }
    //     catch (const InsufficientMoneyException &e)
    //     {
    //         game.getUI().display("Kamu tidak mampu membayar pajak flat M" + std::to_string(flatAmount) + "!");
    //         game.getUI().display("Uang kamu saat ini: M" + std::to_string(player.getWallet().getBalance()));
    //         game.getBankruptcyHandler().handle(player, nullptr, flatAmount);
    //     }
    // }
    // else if (choice == "2")
    // {
    //     int amount = calculateTax(player);

    //     int cash = player.getWallet().getBalance();
    //     int propVal = player.getTotalPropertyValue();
    //     int buildVal = player.getTotalBuildingValue();

    //     game.getUI().display("Total kekayaan kamu:");
    //     game.getUI().display("- Uang tunai          : M" + std::to_string(cash));
    //     game.getUI().display("- Harga beli properti : M" + std::to_string(propVal) + " (termasuk yang digadaikan)");
    //     game.getUI().display("- Harga beli bangunan : M" + std::to_string(buildVal));
    //     game.getUI().display("Total                 : M" + std::to_string(cash + propVal + buildVal));
    //     game.getUI().display("Pajak " + std::to_string(percentage) + "%: M" + std::to_string(amount));

    //     try
    //     {
    //         player.getWallet().deduct(amount);

    //         game.getLogger().log(
    //             game.getCurrentTurn(),
    //             player.getUsername(),
    //             "PAJAK",
    //             "Bayar PPH " + std::to_string(percentage) + "% = M" + std::to_string(amount) + " ke Bank");
    //     }
    //     catch (const InsufficientMoneyException &e)
    //     {
    //         game.getUI().display("Kamu tidak mampu membayar pajak!");
    //         game.getUI().display("Uang kamu saat ini: M" + std::to_string(player.getWallet().getBalance()));
    //         game.getBankruptcyHandler().handle(player, nullptr, amount);
    //     }
    // }
    // else
    // {
    //     game.getUI().display("Pilihan tidak valid. Diasumsikan memilih opsi 1 (flat).");
    //     onLanded(player, game);
    // }
}