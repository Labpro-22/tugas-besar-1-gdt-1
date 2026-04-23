#ifndef COMMANDPROCESSOR_HPP
#define COMMANDPROCESSOR_HPP

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

#include "core/GameEngine.hpp"
#include "core/TurnManager.hpp"
#include "core/Game.hpp"
#include "views/IGUI.hpp"
#include "models/Player/Player.hpp"
#include "models/Property/Property.hpp"
#include "models/Property/StreetProperty.hpp"
#include "models/BoardAndTiles/Board.hpp"
#include "models/BoardAndTiles/Tile.hpp"
#include "models/BoardAndTiles/PropertyTile.hpp"
#include "models/CardAndDeck/SkillCard.hpp"
#include "core/SaveLoadManager.hpp"
#include "utils/data/TransactionLogger.hpp"
#include "utils/data/LogEntry.hpp"

enum class CommandResult {
    CONTINUE,
    END_TURN,
    GAME_OVER,
    SAVED_MID_TURN,
    INVALID
};

class CommandProcessor {
private:
    GameEngine* engine;
    Game*       game;
    TurnManager* turn;
    DiceManager* dice;
    IGUI*       gui;

    std::vector<std::string> tokenize(const std::string& cmd) const;
    std::string normalize(const std::string& s) const;
    bool isAwaitingBonusRoll(Player* player) const;
    bool isAllowedDuringBonusRoll(const std::string& cmd) const;

    CommandResult handleRoll(Player* player, bool manual, int d1, int d2);
    CommandResult handlePrintBoard();
    CommandResult handlePrintDeed(const std::string& code);
    CommandResult handlePrintProperty(Player* player);
    CommandResult handleMortgage(Player* player, const std::string& code);
    CommandResult handleRedeem(Player* player, const std::string& code);
    CommandResult handleBuild(Player* player, const std::string& code);
    CommandResult handleUseSkill(Player* player, int index);
    CommandResult handlePrintLog(int nLast);
    CommandResult handleSave(const std::string& file);
    CommandResult handleFestival(Player* player, const std::string& code);
    CommandResult handleEndTurn(Player* player);
    CommandResult handleHelp(Player* player);

public:
    CommandProcessor(GameEngine* engine, Game* game,
                     TurnManager* turn, DiceManager* dice, IGUI* gui);

    CommandResult process(const std::string& command, Player* player);
};

#endif
