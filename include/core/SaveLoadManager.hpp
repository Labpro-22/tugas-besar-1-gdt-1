#ifndef SAVELOADMANAGER_HPP
#define SAVELOADMANAGER_HPP

#include <string>
#include <vector>
#include "core/Game.hpp"
#include "core/TurnManager.hpp"
#include "views/IGUI.hpp"
#include "utils/data/TransactionLogger.hpp"
#include "models/Player/Player.hpp"
#include "models/Property/Property.hpp"

class SaveLoadManager {
private:
    Game* game;
    TransactionLogger* logger;
    TurnManager* turnManager;
    IGUI* gui;
    std::string configSourceDir;

    static std::string statusToString(PlayerStatus s);
    static PlayerStatus parsePlayerStatus(const std::string& s);
    static std::string propStatusToString(PropertyStatus s);
    static PropertyStatus parsePropStatus(const std::string& s);
    static std::string buildingToString(int stateIdx);
    static int parseBuildingState(const std::string& s);
    static std::vector<std::string> splitBy(const std::string& s, char sep);
    static std::string afterEq(const std::string& kv);
    static std::string resolveSaveDirectory(const std::string& savePath);
    static std::string buildStateFilepath(const std::string& savePath);
    static std::string buildLogFilepath(const std::string& savePath);

    Player* findPlayerByUsername(const std::string& name) const;
    bool saveConfigSnapshot(const std::string& savePath) const;
    bool saveLogFile(const std::string& savePath) const;
    bool loadLogFile(const std::string& savePath, std::vector<LogEntry>& outEntries) const;
    bool saveDeckState(const std::string& savePath) const;

public:
    SaveLoadManager(Game* game, TransactionLogger* logger, TurnManager* turnManager, IGUI* gui,
                    std::string configSourceDir);

    bool saveTargetExists(const std::string& filepath) const;
    bool save(const std::string& filepath);
    bool load(const std::string& filepath);
    bool saveLogSnapshot(const std::string& filepath) const;
};

#endif
