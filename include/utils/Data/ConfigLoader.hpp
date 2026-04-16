#ifndef CONFIGLOADER_HPP
#define CONFIGLOADER_HPP

#include <string>
#include <vector>
#include <map>
#include <tuple>

#include "GameConfig.hpp"

#include "../../models/Property/Property.hpp"
#include "../../models/Property/StreetProperty.hpp"
#include "../../models/Property/RailroadProperty.hpp"
#include "../../models/Property/UtilityProperty.hpp"
#include "../../models/BoardAndTiles/Board.hpp"
#include "../../models/BoardAndTiles/Tile.hpp"
#include "../../models/CardAndDeck/CardDeck.hpp"
#include "../../models/CardAndDeck/ChanceCard.hpp"
#include "../../models/CardAndDeck/CommunityChestCard.hpp"
#include "../../models/CardAndDeck/SkillCard.hpp"

class ConfigLoader {
private:
    std::string configDir;

    std::vector<Property*> loadProperties(std::string filename);
    std::map<int, int> loadRailroadRents(std::string filename);
    std::map<int, int> loadUtilityMultipliers(std::string filename);
    TaxConfig loadTaxConfig(std::string filename);
    SpecialConfig loadSpecialConfig(std::string filename);
    MiscConfig loadMiscConfig(std::string filename);

    StreetProperty* createStreetProperty(std::vector<std::string> tokens);
    RailroadProperty* createRailroadProperty(std::vector<std::string> tokens, const std::map<int, int>& rentTable);
    UtilityProperty* createUtilityProperty(std::vector<std::string> tokens, const std::map<int, int>& multiplierTable);
    Board* createStandardBoard(std::vector<Property*> properties, const GameConfig& config);
    Tile* createTileForIndex(int index, std::vector<Property*> properties, const GameConfig& config);
    
    CardDeck<ChanceCard>* buildChanceDeck();
    CardDeck<CommunityChestCard>* buildCommunityChestDeck();
    CardDeck<SkillCard>* buildSkillDeck();

public:
    ConfigLoader(std::string configDir);
    GameConfig loadGameConfig();
    Board* buildBoard(std::vector<Property*> properties, const GameConfig& config);
    Board* buildDynamicBoard(std::string filename, std::vector<Property*> properties, const GameConfig& config);
    std::tuple<CardDeck<ChanceCard>*, CardDeck<CommunityChestCard>*, CardDeck<SkillCard>*> buildDecks();
};

#endif
