#ifndef SAVELOADMANAGER_HPP
#define SAVELOADMANAGER_HPP

#include <string>
#include "core/Game.hpp"
#include "views/IGUI.hpp"
#include "utils/data/TransactionLogger.hpp"

class SaveLoadManager {
private:
    Game* game;
    TransactionLogger* logger;
    IGUI* gui;

    static std::string statusToString(PlayerStatus s);
    static PlayerStatus parsePlayerStatus(const std::string& s);
    static std::string propStatusToString(PropertyStatus s);
    static PropertyStatus parsePropStatus(const std::string& s);
    static std::string buildingToString(int stateIdx);
    static int parseBuildingState(const std::string& s);

    Player* findPlayerByUsername(const std::string& name) const;

public:
    SaveLoadManager(Game* game, TransactionLogger* logger, IGUI* gui);

    bool save(const std::string& filepath);
    bool load(const std::string& filepath);
};

#endif
