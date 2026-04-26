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

std::string TurnManager::waitForInput(const std::string& prompt) {
    gui->showInputPrompt(prompt);
    while (!gui->shouldExit()) {
        gui->update();
        gui->display();
        std::string command = gui->getCommand();
        if (!command.empty() && command != "NULL") {
            return command;
        }
    }
    return "";
}

void TurnManager::startTurn(Player* player) {
    phase = TurnPhase::START;
    hasActed = false;
    shieldActive = false;

    player->startTurn();
    decrementFestivalDurations();

    phase = TurnPhase::AWAITING_ROLL;
}

void TurnManager::drawSkillCardForTurn(Player* player) {
    distributeSkillCard(player);
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
    if (card == nullptr) return;

    gui->renderSkillDraw(player, card);

    if (player->getCardCount() >= 3) {
        handleDropCard(player, card);
        return;
    }

    player->addCard(card);
    gui->showMessage("Kartu " + card->getCardName() + " masuk ke tangan " + player->getUsername() + ".");
    if (logger != nullptr) {
        logger->log(game->getCurrentTurn(), player->getUsername(),
                    "KARTU", "Mendapat kartu skill: " + card->getCardName());
    }
}

void TurnManager::handleDropCard(Player* player, SkillCard* drawnCard) {
    if (player == nullptr || drawnCard == nullptr || game == nullptr || game->getSkillDeck() == nullptr) {
        return;
    }

    CardDeck<SkillCard>* deck = game->getSkillDeck();
    const auto& hand = player->getHandCards();

    gui->showMessage("Kamu sudah memiliki 3 kartu di tangan. Kamu diwajibkan membuang 1 kartu.");

    int choice = -1;
    const int maxChoice = static_cast<int>(hand.size()) + 1;
    while (!gui->shouldExit()) {
        while (!gui->shouldExit()) {
            gui->update(); gui->display();
            std::string c = gui->getCommand();
            if (!c.empty() && c != "NULL") { 
                choice = stoi(c); break; 
            }
        }
    }

    if (choice == maxChoice) {
        deck->discard(drawnCard);
        gui->showMessage(drawnCard->getCardName() + " dibuang.");
        if (logger != nullptr) {
            logger->log(game->getCurrentTurn(), player->getUsername(),
                        "KARTU", "Mendapat " + drawnCard->getCardName() +
                        ", lalu langsung membuangnya");
        }
        return;
    }

    SkillCard* discarded = hand[choice - 1];
    player->removeCard(discarded);
    deck->discard(discarded);
    player->addCard(drawnCard);

    gui->showMessage(discarded->getCardName() + " dibuang.");
    gui->showMessage("Sekarang kamu memiliki 3 kartu di tangan.");
    if (logger != nullptr) {
        logger->log(game->getCurrentTurn(), player->getUsername(),
                    "KARTU", "Mendapat " + drawnCard->getCardName() +
                    ", buang " + discarded->getCardName());
    }
}

void TurnManager::decrementFestivalDurations() {
    Board* board = game->getBoard();
    if (board == nullptr) return;
    for (Tile* t : board->getAllTiles()) {
        if (t->getCategory() != TileCategory::PROPERTY) continue;
        auto* pt = static_cast<PropertyTile*>(t);
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
