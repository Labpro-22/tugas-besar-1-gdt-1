#include "../../../include/utils/Data/ConfigLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include "../../../include/models/BoardAndTiles/PropertyTile/StreetTile.hpp"
#include "../../../include/models/BoardAndTiles/PropertyTile/RailroadTile.hpp"
#include "../../../include/models/BoardAndTiles/PropertyTile/UtilityTile.hpp"
#include "../../../include/models/BoardAndTiles/SpecialTile/GoTile.hpp"
#include "../../../include/models/BoardAndTiles/SpecialTile/JailTile.hpp"
#include "../../../include/models/BoardAndTiles/SpecialTile/FreeParkingTile.hpp"
#include "../../../include/models/BoardAndTiles/SpecialTile/GoToJailTile.hpp"
#include "../../../include/models/BoardAndTiles/ActionTile/ChanceTile.hpp"
#include "../../../include/models/BoardAndTiles/ActionTile/CommunityChestTile.hpp"
#include "../../../include/models/BoardAndTiles/ActionTile/TaxTile/IncomingTaxTile.hpp"
#include "../../../include/models/BoardAndTiles/ActionTile/TaxTile/LuxuryTaxTile.hpp"

ConfigLoader::ConfigLoader(std::string configDir) : configDir(configDir) {}

GameConfig ConfigLoader::loadGameConfig() {
    GameConfig config;
    
    config.setRailroadRents(loadRailroadRents(configDir + "/railroad.txt"));
    config.setUtilityMultipliers(loadUtilityMultipliers(configDir + "/utility.txt"));
    config.setTax(loadTaxConfig(configDir + "/tax.txt"));
    config.setSpecial(loadSpecialConfig(configDir + "/special.txt"));
    config.setMisc(loadMiscConfig(configDir + "/misc.txt"));
    
    config.setProperties(loadProperties(configDir + "/property.txt"));
    
    return config;
}

std::vector<Property*> ConfigLoader::loadProperties(std::string filename) {
    std::vector<Property*> properties(40, nullptr); // Create 40 slots with nullptr
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open " << filename << std::endl;
        return properties;
    }
    
    std::string line;
    // Skip header line
    std::getline(file, line);
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (ss >> token) {
            tokens.push_back(token);
        }
        
        if (tokens.size() < 5) continue;
        
        int id = std::stoi(tokens[0]);
        if (id < 0 || id >= 40) continue; // Out of bounds safety
        
        std::string jenis = tokens[3];
        if (jenis == "STREET") {
            properties[id] = createStreetProperty(tokens);
        } else if (jenis == "RAILROAD") {
            std::map<int, int> rTable = loadRailroadRents(configDir + "/railroad.txt");
            properties[id] = createRailroadProperty(tokens, rTable);
        } else if (jenis == "UTILITY") {
            std::map<int, int> uTable = loadUtilityMultipliers(configDir + "/utility.txt");
            properties[id] = createUtilityProperty(tokens, uTable);
        }
    }
    return properties;
}

std::map<int, int> ConfigLoader::loadRailroadRents(std::string filename) {
    std::map<int, int> rentTable;
    std::ifstream file(filename);
    if (!file.is_open()) return rentTable;
    
    std::string line;
    std::getline(file, line);
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        int count, rent;
        if (ss >> count >> rent) {
            rentTable[count] = rent;
        }
    }
    return rentTable;
}

std::map<int, int> ConfigLoader::loadUtilityMultipliers(std::string filename) {
    std::map<int, int> multipliers;
    std::ifstream file(filename);
    if (!file.is_open()) return multipliers;
    
    std::string line;
    std::getline(file, line);
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        int count, mult;
        if (ss >> count >> mult) {
            multipliers[count] = mult;
        }
    }
    return multipliers;
}

TaxConfig ConfigLoader::loadTaxConfig(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return TaxConfig();
    
    std::string line;
    std::getline(file, line);
    std::getline(file, line);
    std::stringstream ss(line);
    int pphFlat, pphPercent, pbmFlat;
    ss >> pphFlat >> pphPercent >> pbmFlat;
    return TaxConfig(pphFlat, pphPercent, pbmFlat);
}

SpecialConfig ConfigLoader::loadSpecialConfig(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return SpecialConfig();
    
    std::string line;
    std::getline(file, line);
    std::getline(file, line);
    std::stringstream ss(line);
    int goSalary, jailFine;
    ss >> goSalary >> jailFine;
    return SpecialConfig(goSalary, jailFine);
}

MiscConfig ConfigLoader::loadMiscConfig(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return MiscConfig();
    
    std::string line;
    std::getline(file, line);
    std::getline(file, line);
    std::stringstream ss(line);
    int maxTurn, initialBalance;
    ss >> maxTurn >> initialBalance;
    return MiscConfig(maxTurn, initialBalance);
}

StreetProperty* ConfigLoader::createStreetProperty(std::vector<std::string> tokens) {
    std::string code = tokens[1];
    std::string name = tokens[2];
    std::string colorGroup = tokens[4];
    int purchasePrice = std::stoi(tokens[5]);
    int mortgageValue = std::stoi(tokens[6]);
    int houseBuildCost = std::stoi(tokens[7]);
    int hotelBuildCost = std::stoi(tokens[8]);
    
    std::vector<int> rentLevels;
    for (size_t i = 9; i < tokens.size() && i < 15; ++i) {
        rentLevels.push_back(std::stoi(tokens[i]));
    }
    
    return new StreetProperty(code, name, purchasePrice, mortgageValue, colorGroup, houseBuildCost, hotelBuildCost, rentLevels);
}

RailroadProperty* ConfigLoader::createRailroadProperty(std::vector<std::string> tokens, const std::map<int, int>& rentTable) {
    std::string code = tokens[1];
    std::string name = tokens[2];
    int purchasePrice = std::stoi(tokens[5]);
    int mortgageValue = std::stoi(tokens[6]);
    
    return new RailroadProperty(code, name, purchasePrice, mortgageValue, rentTable);
}

UtilityProperty* ConfigLoader::createUtilityProperty(std::vector<std::string> tokens, const std::map<int, int>& multiplierTable) {
    std::string code = tokens[1];
    std::string name = tokens[2];
    int purchasePrice = std::stoi(tokens[5]);
    int mortgageValue = std::stoi(tokens[6]);
    
    return new UtilityProperty(code, name, purchasePrice, mortgageValue, multiplierTable);
}

Board* ConfigLoader::buildBoard(std::vector<Property*> properties, const GameConfig& config) {
    return createStandardBoard(properties, config);
}

Board* ConfigLoader::buildDynamicBoard(std::string filename, std::vector<Property*> properties, const GameConfig& config) {
    // Unsupported dynamically so fallback to standard
    return buildBoard(properties, config);
}

std::tuple<CardDeck<ChanceCard>*, CardDeck<CommunityChestCard>*, CardDeck<SkillCard>*> ConfigLoader::buildDecks() {
    return { buildChanceDeck(), buildCommunityChestDeck(), buildSkillDeck() };
}

Board* ConfigLoader::createStandardBoard(std::vector<Property*> properties, const GameConfig& config) {
    Board* board = new Board();
    for (int i = 0; i < 40; ++i) {
        Tile* newTile = createTileForIndex(i, properties, config);
        if (newTile != nullptr) {
            board->addTile(newTile);
        }
    }
    return board;
}

Tile* ConfigLoader::createTileForIndex(int index, std::vector<Property*> properties, const GameConfig& config) {
    // 1. Check if it's a Property from the vector
    if (index >= 0 && index < (int)properties.size() && properties[index] != nullptr) {
        Property* prop = properties[index];
        if (prop->getType() == PropertyType::STREET) {
            auto streetProp = dynamic_cast<StreetProperty*>(prop);
            TileColor color = TileColor::DEFAULT;
            std::string c = streetProp->getColorGroup();
            if (c == "COKLAT") color = TileColor::COKLAT;
            else if (c == "BIRU_MUDA") color = TileColor::BIRU_MUDA;
            else if (c == "MERAH_MUDA") color = TileColor::MERAH_MUDA;
            else if (c == "ORANGE") color = TileColor::ORANGE;
            else if (c == "MERAH") color = TileColor::MERAH;
            else if (c == "KUNING") color = TileColor::KUNING;
            else if (c == "HIJAU") color = TileColor::HIJAU;
            else if (c == "BIRU_TUA") color = TileColor::BIRU_TUA;
            else if (c == "ABU_ABU") color = TileColor::ABU_ABU;
            return new StreetTile(index, prop->getCode(), prop->getName(), color, streetProp);
        } else if (prop->getType() == PropertyType::RAILROAD) {
            return new RailroadTile(index, prop->getCode(), prop->getName(), dynamic_cast<RailroadProperty*>(prop));
        } else if (prop->getType() == PropertyType::UTILITY) {
            return new UtilityTile(index, prop->getCode(), prop->getName(), dynamic_cast<UtilityProperty*>(prop));
        }
    }
    
    switch (index) {
        case 0:  return new GoTile(index, config.getSpecial().getGoSalary());
        case 10: return new JailTile(index);
        case 20: return new FreeParkingTile(index);
        case 30: return new GoToJailTile(index);
        case 2:
        case 17:
        case 33: return new CommunityChestTile(index);
        case 7:
        case 22:
        case 36: return new ChanceTile(index);
        case 4:  return new IncomeTaxTile(index, config.getTax().getPphFlat(), config.getTax().getPphPercent());
        case 38: return new LuxuryTaxTile(index, config.getTax().getPbmFlat());
        default: return nullptr; // empty tile
    }
}

CardDeck<ChanceCard>* ConfigLoader::buildChanceDeck() {
    CardDeck<ChanceCard>* deck = new CardDeck<ChanceCard>();
    // Add stub chance cards if needed
    return deck;
}

CardDeck<CommunityChestCard>* ConfigLoader::buildCommunityChestDeck() {
    CardDeck<CommunityChestCard>* deck = new CardDeck<CommunityChestCard>();
    // Add stub CC cards if needed
    return deck;
}

CardDeck<SkillCard>* ConfigLoader::buildSkillDeck() {
    CardDeck<SkillCard>* deck = new CardDeck<SkillCard>();
    return deck;
}
