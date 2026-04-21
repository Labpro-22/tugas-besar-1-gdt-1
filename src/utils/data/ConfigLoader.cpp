#include "utils/data/ConfigLoader.hpp"

// Constructor
ConfigLoader::ConfigLoader(std::string configDir) : configDir(configDir) {}

// API untuk Game
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

// load file untuk ke Objek Property (Property.txt, Railroad.txt, Utility.txt)
std::vector<Property*> ConfigLoader::loadProperties(std::string filename) {
    std::vector<Property*> properties(40, nullptr);
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open " << filename << std::endl;
        return properties;
    }
    std::map<int, int> rTable = loadRailroadRents(configDir + "/railroad.txt");
    std::map<int, int> uTable = loadUtilityMultipliers(configDir + "/utility.txt");

    std::string line;
    std::getline(file, line); // Skip header line
    
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
        if (id < 1 || id > 40) continue;
        int idx = id - 1;

        std::string jenis = tokens[3];
        if (jenis == "STREET") {
            properties[idx] = createStreetProperty(tokens);
        } else if (jenis == "RAILROAD") {
            properties[idx] = createRailroadProperty(tokens, rTable);
        } else if (jenis == "UTILITY") {
            properties[idx] = createUtilityProperty(tokens, uTable);
        }
    }
    return properties;
}

// Load file railroad.txt  
std::map<int, int> ConfigLoader::loadRailroadRents(std::string filename) {
    std::map<int, int> rentTable;
    std::ifstream file(filename);
    if (!file.is_open()) return rentTable;
    
    std::string line;
    std::getline(file, line); // skip header
    
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

// Load file utility.txt
std::map<int, int> ConfigLoader::loadUtilityMultipliers(std::string filename) {
    std::map<int, int> multipliers;
    std::ifstream file(filename);
    if (!file.is_open()) return multipliers;
    
    std::string line;
    std::getline(file, line); // skip header
    
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

// Load file tax.txt
TaxConfig ConfigLoader::loadTaxConfig(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return TaxConfig();
    
    std::string line;
    std::getline(file, line); // skip header
    std::getline(file, line); // data row
    std::stringstream ss(line);
    int pphFlat, pphPercent, pbmFlat;
    ss >> pphFlat >> pphPercent >> pbmFlat;
    return TaxConfig(pphFlat, pphPercent, pbmFlat);
}

// Load file special.txt
SpecialConfig ConfigLoader::loadSpecialConfig(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return SpecialConfig();
    
    std::string line;
    std::getline(file, line); // skip header
    std::getline(file, line); // data row
    std::stringstream ss(line);
    int goSalary, jailFine;
    ss >> goSalary >> jailFine;
    return SpecialConfig(goSalary, jailFine);
}

// Load file misc.txt
MiscConfig ConfigLoader::loadMiscConfig(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return MiscConfig();
    
    std::string line;
    std::getline(file, line); // skip header
    std::getline(file, line); // data row
    std::stringstream ss(line);
    int maxTurn, initialBalance;
    ss >> maxTurn >> initialBalance;
    return MiscConfig(maxTurn, initialBalance);
}

// Convert data into Street Property objects
StreetProperty* ConfigLoader::createStreetProperty(std::vector<std::string> tokens) {
    // Format: ID KODE NAMA JENIS WARNA HARGA_LAHAN NILAI_GADAI UPG_RUMAH UPG_HT RENT_L0…RENT_L5
    std::string code = tokens[1];
    std::string name = tokens[2];
    std::string colorGroup = tokens[4];
    int purchasePrice = std::stoi(tokens[5]);
    int mortgageValue = std::stoi(tokens[6]);
    int houseBuildCost = std::stoi(tokens[7]);
    int hotelBuildCost = std::stoi(tokens[8]);
    
    std::vector<int> rentLevels;
    for (size_t i = 9; i < tokens.size() && i < 15; ++i) {
        try { rentLevels.push_back(std::stoi(tokens[i])); }
        catch (...) { continue; }
    }
    
    return new StreetProperty(code, name, purchasePrice, mortgageValue,
                              colorGroup, houseBuildCost, hotelBuildCost, rentLevels);
}

// Convert data into Railroad Property objects
RailroadProperty* ConfigLoader::createRailroadProperty(
        std::vector<std::string> tokens, const std::map<int, int>& rentTable) {
    // Format: ID KODE NAMA JENIS WARNA HARGA_LAHAN NILAI_GADAI
    std::string code = tokens[1];
    std::string name = tokens[2];
    int purchasePrice = std::stoi(tokens[5]);
    int mortgageValue = std::stoi(tokens[6]);
    
    return new RailroadProperty(code, name, purchasePrice, mortgageValue, rentTable);
}

// Convert data into Utility Property objects
UtilityProperty* ConfigLoader::createUtilityProperty(
        std::vector<std::string> tokens, const std::map<int, int>& multiplierTable) {
    std::string code = tokens[1];
    std::string name = tokens[2];
    int purchasePrice = std::stoi(tokens[5]);
    int mortgageValue = std::stoi(tokens[6]);
    
    return new UtilityProperty(code, name, purchasePrice, mortgageValue, multiplierTable);
}

// Memeritahkan untuk membuat Board dari data Property yang sudah di-load
Board* ConfigLoader::buildBoard(std::vector<Property*> properties, const GameConfig& config) {
    return createStandardBoard(properties, config);
}


Board* ConfigLoader::buildDynamicBoard(std::string /*filename*/,
        std::vector<Property*> properties, const GameConfig& config) {
    // Dynamic board loading not yet supported; fallback to standard
    return buildBoard(properties, config);
}

Board* ConfigLoader::createStandardBoard(std::vector<Property*> properties, const GameConfig& config) {
    Board* board = new Board();
    for (int i = 0; i < 40; ++i) {
        Tile* tile = createTileForIndex(i, properties, config);
        if (tile != nullptr) {
            board->addTile(tile);
        }
    }
    return board;
}

//  Standard Nimonspoli layout (40 tiles):
//   0  GO
//   1  StreetProperty (COKLAT)
//   2  Community Chest
//   3  StreetProperty (COKLAT)
//   4  Pajak Penghasilan (Income Tax)
//   5  RailroadProperty
//   6  StreetProperty (BIRU_MUDA)
//   7  Chance
//   8  StreetProperty (BIRU_MUDA)
//   9  StreetProperty (BIRU_MUDA)
//  10  Penjara / Just Visiting
//  11  StreetProperty (MERAH_MUDA)
//  12  UtilityProperty
//  13  StreetProperty (MERAH_MUDA)
//  14  StreetProperty (MERAH_MUDA)
//  15  RailroadProperty
//  16  StreetProperty (ORANGE)
//  17  Community Chest
//  18  StreetProperty (ORANGE)
//  19  StreetProperty (ORANGE)
//  20  Free Parking / Festival
//  21  StreetProperty (MERAH)
//  22  Chance
//  23  StreetProperty (MERAH)
//  24  StreetProperty (MERAH)
//  25  RailroadProperty
//  26  StreetProperty (KUNING)
//  27  StreetProperty (KUNING)
//  28  UtilityProperty
//  29  StreetProperty (KUNING)
//  30  Go To Jail
//  31  StreetProperty (HIJAU)
//  32  StreetProperty (HIJAU)
//  33  Community Chest
//  34  StreetProperty (HIJAU)
//  35  RailroadProperty
//  36  Chance
//  37  StreetProperty (BIRU_TUA)
//  38  Pajak Barang Mewah (Luxury Tax)
//  39  StreetProperty (BIRU_TUA)

Tile* ConfigLoader::createTileForIndex(int index,
        std::vector<Property*> properties, const GameConfig& config) {

    // a PropertyTile
    if (index >= 0 && index < (int)properties.size() && properties[index] != nullptr) {
        Property* prop = properties[index];

        if (prop->getType() == PropertyType::STREET) {
            auto* streetProp = dynamic_cast<StreetProperty*>(prop);
            // Map colorGroup string → TileColor enum
            TileColor color = TileColor::DEFAULT;
            std::string c = streetProp->getColorGroup();
            if      (c == "COKLAT")    color = TileColor::COKLAT;
            else if (c == "BIRU_MUDA") color = TileColor::BIRU_MUDA;
            else if (c == "MERAH_MUDA")color = TileColor::MERAH_MUDA;
            else if (c == "ORANGE")    color = TileColor::ORANYE;
            else if (c == "MERAH")     color = TileColor::MERAH;
            else if (c == "KUNING")    color = TileColor::KUNING;
            else if (c == "HIJAU")     color = TileColor::HIJAU;
            else if (c == "BIRU_TUA")  color = TileColor::BIRU_TUA;
            else if (c == "ABU_ABU")   color = TileColor::ABU_ABU;
            return new StreetTile(index, prop->getCode(), prop->getName(),color, streetProp);

        } else if (prop->getType() == PropertyType::RAILROAD) {
            return new RailroadTile(index, prop->getCode(), prop->getName(),
                                    dynamic_cast<RailroadProperty*>(prop));

        } else if (prop->getType() == PropertyType::UTILITY) {
            return new UtilityTile(index, prop->getCode(), prop->getName(),dynamic_cast<UtilityProperty*>(prop));
        }
    }
    
    // Non-property tiles (Nimonspoli 0-based layout)
    switch (index) {
        case 0:  return new GoTile(index, config.getSpecial().getGoSalary());   // GO
        case 10: return new JailTile(index);                                    // PEN
        case 20: return new FreeParkingTile(index);                             // BBP
        case 30: return new GoToJailTile(index);                                // PPJ

        case 2:
        case 17: return new CommunityChestTile(index);                          // DNU

        case 22:
        case 36: return new ChanceTile(index);                                  // KSP

        case 7:
        case 33: return new FestivalTile(index);                                // FES

        case 4:  return new IncomeTaxTile(index, config.getTax().getPphFlat(),
                                          config.getTax().getPphPercent());     // PPH
        case 38: return new LuxuryTaxTile(index, config.getTax().getPbmFlat()); // PBM

        default: return nullptr;
    }
}

//  Deck building
std::tuple<CardDeck<ChanceCard>*, CardDeck<CommunityChestCard>*, CardDeck<SkillCard>*>
ConfigLoader::buildDecks() {
    return { buildChanceDeck(), buildCommunityChestDeck(), buildSkillDeck() };
}

CardDeck<ChanceCard>* ConfigLoader::buildChanceDeck() {
    auto* deck = new CardDeck<ChanceCard>();
    // One card of each ChanceType
    deck->addCard(new ChanceCard(ChanceType::GO_TO_NEAREST_STATION));
    deck->addCard(new ChanceCard(ChanceType::MOVE_BACK_3));
    deck->addCard(new ChanceCard(ChanceType::GO_TO_JAIL));
    deck->shuffle();
    return deck;
}

CardDeck<CommunityChestCard>* ConfigLoader::buildCommunityChestDeck() {
    auto* deck = new CardDeck<CommunityChestCard>();
    // One card of each CommunityType
    deck->addCard(new CommunityChestCard(CommunityType::BIRTHDAY));
    deck->addCard(new CommunityChestCard(CommunityType::DOCTOR_FEE));
    deck->addCard(new CommunityChestCard(CommunityType::CAMPAIGN_FEE));
    deck->shuffle();
    return deck;
}

CardDeck<SkillCard>* ConfigLoader::buildSkillDeck() {
    auto* deck = new CardDeck<SkillCard>();
    // One card of each SkillCard type
    deck->addCard(new MoveCard(3));
    deck->addCard(new DiscountCard(50));
    deck->addCard(new ShieldCard());
    deck->addCard(new TeleportCard());
    deck->addCard(new LassoCard());
    deck->addCard(new DemolitionCard());
    deck->shuffle();
    return deck;
}