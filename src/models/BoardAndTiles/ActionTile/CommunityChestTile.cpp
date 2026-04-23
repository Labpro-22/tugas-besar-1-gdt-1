#include "models/BoardAndTiles/ActionTile/CommunityChestTile.hpp"
#include "models/CardAndDeck/CommunityChestCard.hpp"
#include "core/Game.hpp"
#include "models/Player/Player.hpp"
#include <iostream>

CommunityChestTile::CommunityChestTile(int index)
    : ActionTile(index, "DNU", "Dana Umum") {}

void CommunityChestTile::onLanded(Player &player, Game &game)
{
    if (game.getCommunityDeck() == nullptr || game.getCommunityDeck()->isEmpty()) return;

    CommunityChestCard *card = game.getCommunityDeck()->draw();
    if (card == nullptr) return;

    std::cout << player.getUsername() << " mengambil kartu Dana Umum: " << card->getDescription() << "\n";

    switch (card->getType()) {
        case CommunityType::BIRTHDAY: {
            // Kumpulkan M100 dari setiap pemain aktif lain
            for (Player *other : game.getActivePlayers()) {
                if (other == &player) continue;
                int amount = 100;
                if (other->canAfford(amount)) {
                    other->deductMoney(amount);
                    player.addMoney(amount);
                    std::cout << other->getUsername() << " memberi M" << amount
                              << " kepada " << player.getUsername() << " (ulang tahun).\n";
                }
            }
            break;
        }
        case CommunityType::DOCTOR_FEE: {
            int fee = 700;
            if (player.canAfford(fee)) {
                player.deductMoney(fee);
                std::cout << player.getUsername() << " membayar biaya dokter M" << fee << ".\n";
            } else {
                player.setStatus(PlayerStatus::BANKRUPT);
            }
            break;
        }
        case CommunityType::CAMPAIGN_FEE: {
            // Bayar M200 ke setiap pemain aktif lain
            int fee = 200;
            for (Player *other : game.getActivePlayers()) {
                if (other == &player) continue;
                if (player.canAfford(fee)) {
                    player.deductMoney(fee);
                    other->addMoney(fee);
                    std::cout << player.getUsername() << " membayar M" << fee
                              << " ke " << other->getUsername() << " (biaya kampanye).\n";
                }
            }
            break;
        }
    }

    game.getCommunityDeck()->discard(card);
}