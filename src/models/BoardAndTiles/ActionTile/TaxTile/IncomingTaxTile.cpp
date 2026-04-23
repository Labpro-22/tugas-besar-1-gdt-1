#include "models/BoardAndTiles/ActionTile/TaxTile/IncomingTaxTile.hpp"
#include "models/Player/Player.hpp"
#include <iostream>
#include <algorithm>

IncomeTaxTile::IncomeTaxTile(int index, int flatAmount, int percentage)
    : TaxTile(index, "PPH", "Pajak Penghasilan"),
      flatAmount(flatAmount),
      percentage(percentage) {}

const int IncomeTaxTile::getFlatAmount() const {
    return flatAmount;
}

const int IncomeTaxTile::getTaxPercentage() const {
    return percentage;
}

int IncomeTaxTile::calculateTax(const Player &player) const {
    // 10% dari total aset pemain (menggunakan calculateTotalWealth)
    int percentageTax = (player.calculateTotalWealth() * percentage) / 100;

    // Spesifikasi Nimonspoli: bayar yang LEBIH KECIL antara flatAmount atau persentase
    return std::min(flatAmount, percentageTax);
}

void IncomeTaxTile::onLanded(Player &player, Game & /*game*/) {
    int taxToPay = calculateTax(player);

    std::cout << player.getUsername() << " mendarat di Pajak Penghasilan!\n";

    if (!player.canAfford(taxToPay)) {
        // TODO: lempar InsufficientTaxException saat exception kustom tersedia
        // (InsufficientMoneyException butuh Property* yang tidak ada di TaxTile)
        std::cout << "Saldo tidak cukup untuk bayar pajak " << taxToPay
                  << ". Pemain berpotensi bangkrut.\n";
        return;
    }

    player.deductMoney(taxToPay);
    std::cout << "Membayar pajak sebesar " << taxToPay << ".\n";
}
