#include "models/BoardAndTiles/PropertyTile/StreetTile.hpp"
#include "models/Property/StreetProperty.hpp"
// #include "core/Game.hpp"
#include "models/Player/Player.hpp"
#include "exception/PlayerTurn/PropertyManagement/InsufficientMoneyException.hpp"
#include <iostream>

StreetTile::StreetTile(int index, const std::string &code, const std::string &name, TileColor color, StreetProperty *street)
    : PropertyTile(index, code, name, color, street) {}

void StreetTile::onLanded(Player &player, Game &game) {
    // 1. Ambil objek Property yang tersimpan di dalam Tile ini
    Property* prop = this->getProperty(); 

    // 2. Cek apakah properti sudah ada pemiliknya
    if (prop->isOwned()) {
        Player* owner = prop->getOwner(); // Sekarang getOwner() valid!

        // 3. Cek apakah yang menginjak bukan pemiliknya sendiri, dan properti tidak digadai
        if (owner != &player && !prop->isMortgaged()) {
            
            // 4. Hitung sewa (asumsi tidak ada dadu khusus untuk street, jadi kosongi/default 0)
            int rentCost = prop->calculateRent(); // Sekarang calculateRent() valid!

            // 5. PENANGANAN EKSEPSI JIKA UANG TIDAK CUKUP
            if (!player.canAfford(rentCost)) {
                // Kita pass pointer player, pointer property, dan harga sewanya
                throw InsufficientMoneyException(&player, prop, rentCost); 
            }

            // 6. Jika uang cukup, lakukan transaksi normal
            player.deductMoney(rentCost);
            owner->addMoney(rentCost);

            std::cout << player.getUsername() << " mendarat di " << prop->getName() 
                      << " milik " << owner->getUsername() << ".\n";
            std::cout << player.getUsername() << " otomatis membayar sewa sebesar " << rentCost << ".\n";
        } 
        else if (owner == &player) {
            std::cout << player.getUsername() << " mendarat di propertinya sendiri (" << prop->getName() << ").\n";
        } 
        else if (prop->isMortgaged()) {
            std::cout << prop->getName() << " sedang digadaikan. " << player.getUsername() << " bebas dari uang sewa!\n";
        }
    } else {
        // Jika belum dimiliki (opsional dicetak, karena spesifikasi M2 biasanya ada command BELI manual)
        std::cout << player.getUsername() << " mendarat di " << prop->getName() << " yang belum dimiliki siapa pun.\n";
    }
}