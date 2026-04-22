#ifndef CONFIGLOADER_HPP
#define CONFIGLOADER_HPP

// STL headers
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

// Class headers
#include "utils/data/GameConfig.hpp"

#include "models/Property/Property.hpp"
#include "models/Property/StreetProperty.hpp"
#include "models/Property/RailroadProperty.hpp"
#include "models/Property/UtilityProperty.hpp"
#include "models/BoardAndTiles/Board.hpp"
#include "models/BoardAndTiles/Tile.hpp"
#include "models/CardAndDeck/CardDeck.hpp"

// Tile headers for board construction
#include "models/BoardAndTiles/PropertyTile/StreetTile.hpp"
#include "models/BoardAndTiles/PropertyTile/RailroadTile.hpp"
#include "models/BoardAndTiles/PropertyTile/UtilityTile.hpp"
#include "models/BoardAndTiles/SpecialTile/GoTile.hpp"
#include "models/BoardAndTiles/SpecialTile/JailTile.hpp"
#include "models/BoardAndTiles/SpecialTile/FreeParkingTile.hpp"
#include "models/BoardAndTiles/SpecialTile/GoToJailTile.hpp"
#include "models/BoardAndTiles/ActionTile/ChanceTile.hpp"
#include "models/BoardAndTiles/ActionTile/CommunityChestTile.hpp"
#include "models/BoardAndTiles/ActionTile/FestivalTile.hpp"
#include "models/BoardAndTiles/ActionTile/TaxTile/IncomingTaxTile.hpp"
#include "models/BoardAndTiles/ActionTile/TaxTile/LuxuryTaxTile.hpp"

// Card headers for deck construction
#include "models/CardAndDeck/MoveCard.hpp"
#include "models/CardAndDeck/DiscountCard.hpp"
#include "models/CardAndDeck/ShieldCard.hpp"
#include "models/CardAndDeck/TeleportCard.hpp"
#include "models/CardAndDeck/LassoCard.hpp"
#include "models/CardAndDeck/DemolitionCard.hpp"
#include "models/CardAndDeck/ChanceCard.hpp"
#include "models/CardAndDeck/CommunityChestCard.hpp"
#include "models/CardAndDeck/SkillCard.hpp"

class ConfigLoader {
private:
    std::string configDir;

    std::vector<Property*> loadProperties(std::string filename);
    std::map<int, int> loadRailroadRents(std::string filename);
    std::map<int, int> loadUtilityMultipliers(std::string filename);
    std::map<int, ActionTileConfig> loadActionTiles(std::string filename);
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