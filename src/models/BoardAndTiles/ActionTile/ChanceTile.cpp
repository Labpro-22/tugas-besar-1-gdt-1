#include "models/BoardAndTiles/ActionTile/ChanceTile.hpp"
#include "models/CardAndDeck/ChanceCard.hpp"
#include "models/BoardAndTiles/Board.hpp"
#include "models/BoardAndTiles/PropertyTile/RailroadTile.hpp"
#include "models/BoardAndTiles/SpecialTile/JailTile.hpp"
#include "core/Game.hpp"
#include "models/Player/Player.hpp"
#include <iostream>

ChanceTile::ChanceTile(int index)
    : ActionTile(index, "KSP", "Kesempatan") {}

void ChanceTile::onLanded(Player &player, Game &game)
{
    if (game.getChanceDeck() == nullptr || game.getChanceDeck()->isEmpty()) return;

    ChanceCard *card = game.getChanceDeck()->draw();
    if (card == nullptr) return;

    std::cout << player.getUsername() << " mengambil kartu Kesempatan: " << card->getDescription() << "\n";

    switch (card->getType()) {
        case ChanceType::GO_TO_NEAREST_STATION: {
            Board *board = game.getBoard();
            if (board != nullptr) {
                RailroadTile *nearest = board->getNearestRailroad(player.getPosition());
                if (nearest != nullptr) {
                    player.setPosition(nearest->getIndex());
                    std::cout << player.getUsername() << " pindah ke stasiun terdekat (petak "
                              << nearest->getIndex() << ").\n";
                    nearest->onLanded(player, game);
                }
            }
            break;
        }
        case ChanceType::MOVE_BACK_3: {
            Board *board = game.getBoard();
            int boardSize = board ? (int)board->getAllTiles().size() : 40;
            int newPos = (player.getPosition() - 3 + boardSize) % boardSize;
            player.setPosition(newPos);
            std::cout << player.getUsername() << " mundur 3 petak ke petak " << newPos << ".\n";
            if (board != nullptr) {
                Tile *t = board->getTile(newPos);
                if (t != nullptr) t->onLanded(player, game);
            }
            break;
        }
        case ChanceType::GO_TO_JAIL: {
            player.setStatus(PlayerStatus::JAILED);
            Board *board = game.getBoard();
            if (board != nullptr && board->getJailTile() != nullptr) {
                player.setPosition(board->getJailTile()->getIndex());
            }
            std::cout << player.getUsername() << " masuk Penjara!\n";
            break;
        }
    }

    game.getChanceDeck()->discard(card);
}