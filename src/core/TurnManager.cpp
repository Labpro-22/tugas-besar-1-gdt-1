#include "core/TurnManager.hpp"

#include "core/Game.hpp"
#include "models/Player/Player.hpp"
#include "models/BoardAndTiles/Board.hpp"
#include "models/BoardAndTiles/Tile.hpp"
#include "models/BoardAndTiles/SpecialTile/JailTile.hpp"
#include "models/BoardAndTiles/PropertyTile.hpp"
#include "models/Property/Property.hpp"
#include "views/IGUI.hpp"
#include "utils/data/TransactionLogger.hpp"
#include "models/CardAndDeck/SkillCard.hpp"

TurnManager::TurnManager(Game* game, DiceManager* dice, IGUI* gui, TransactionLogger* logger)
    : game(game), dice(dice), gui(gui), logger(logger),
      phase(TurnPhase::START), hasActed(false), shieldActive(false) {}

void TurnManager::startTurn(Player* player) {
    phase = TurnPhase::START;
    hasActed = false;
    shieldActive = false;

    player->startTurn();
    distributeSkillCard(player);
    handleDropCard(player);
    decrementFestivalDurations();

    phase = TurnPhase::AWAITING_ROLL;
}

void TurnManager::endTurn(Player* player) {
    if (player != nullptr) {
        player->resetConsecutiveDoubles();
    }

    hasActed = false;
    shieldActive = false;
    phase = TurnPhase::ENDED;
}

bool TurnManager::canRoll(Player* player) const {
    if (player == nullptr || player->hasPendingFestival()) {
        return false;
    }
    if (!player->hasRolled()) {
        return true;
    }
    return phase == TurnPhase::POST_ROLL && player->getConsecutiveDoubles() > 0;
}

bool TurnManager::canUseSkill(Player* player) const {
    return !player->hasRolled() && !player->hasUsedSkill();
}

Tile* TurnManager::processMovement(Player* player, int steps) {
    Board* board = game->getBoard();
    if (board == nullptr) return nullptr;

    int from = player->getPosition();
    if (board->passesGo(from, steps)) {
        player->addMoney(game->getGoSalary());
    }
    int next = board->getNextIndex(from, steps);
    player->setPosition(next);

    phase = TurnPhase::POST_ROLL;
    return board->getTile(next);
}

void TurnManager::distributeSkillCard(Player* player) {
    CardDeck<SkillCard>* deck = game->getSkillDeck();
    if (deck == nullptr || deck->isEmpty()) return;
    SkillCard* card = deck->draw();
    if (!player->addCard(card)) {
        deck->discard(card);
        return;
    }
    if (logger != nullptr) {
        logger->log(game->getCurrentTurn(), player->getUsername(),
                    "KARTU", "Mendapat kartu skill: " + card->getCardName());
    }
}

void TurnManager::handleDropCard(Player* player) {
    const int HAND_LIMIT = 5;
    while (player->getCardCount() > HAND_LIMIT) {
        const auto& hand = player->getHandCards();
        if (hand.empty()) break;
        SkillCard* drop = hand.front();
        player->removeCard(drop);
        game->getSkillDeck()->discard(drop);
        // TODO: ganti ke pemilihan via IGUI ketika API pilih-kartu tersedia
    }
}

void TurnManager::decrementFestivalDurations() {
    Board* board = game->getBoard();
    if (board == nullptr) return;
    for (Tile* t : board->getAllTiles()) {
        auto* pt = dynamic_cast<PropertyTile*>(t);
        if (pt == nullptr) continue;
        Property* prop = pt->getProperty();
        if (prop != nullptr) prop->decrementFestivalDuration();
    }
}

TurnPhase TurnManager::getPhase() const { return phase; }
void TurnManager::setPhase(TurnPhase p) { phase = p; }

bool TurnManager::isShieldActive() const { return shieldActive; }
void TurnManager::setShieldActive(bool v) { shieldActive = v; }

bool TurnManager::hasActedThisTurn() const { return hasActed; }
void TurnManager::markActed() { hasActed = true; }
