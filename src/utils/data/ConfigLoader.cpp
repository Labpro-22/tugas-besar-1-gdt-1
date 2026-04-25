#include "utils/data/ConfigLoader.hpp"
#include "models/BoardAndTiles/TileTypes.hpp"
#include "exception/InvalidFile/FileNotFoundException.hpp"
#include "exception/InvalidFile/UnreadableFileException.hpp"
#include "exception/InvalidFile/InvalidConfigException.hpp"

// Constructor
ConfigLoader::ConfigLoader(std::string configDir) : configDir(configDir) {}

GameConfig ConfigLoader::loadGameConfig() {
    GameConfig config;

    config.setRailroadRents(loadRailroadRents(configDir + "/railroad.txt"));
    config.setUtilityMultipliers(loadUtilityMultipliers(configDir + "/utility.txt"));
    config.setTax(loadTaxConfig(configDir + "/tax.txt"));
    config.setSpecial(loadSpecialConfig(configDir + "/special.txt"));
    config.setMisc(loadMiscConfig(configDir + "/misc.txt"));
    config.setProperties(loadProperties(configDir + "/property.txt"));
    config.setActionTiles(loadActionTiles(configDir + "/aksi.txt"));

    return config;
}

// load file untuk ke Objek Property (Property.txt, Railroad.txt, Utility.txt)
std::vector<Property*> ConfigLoader::loadProperties(std::string filename) {
    std::vector<Property*> properties(40, nullptr);

    std::ifstream file(filename);
    if (!file.is_open()) {
        throw FileNotFoundException(filename);
    }

    std::map<int, int> rTable = loadRailroadRents(configDir + "/railroad.txt");
    std::map<int, int> uTable = loadUtilityMultipliers(configDir + "/utility.txt");

    std::string line;
    if (!std::getline(file, line))
        throw InvalidConfigException(filename, "Header property.txt tidak valid");

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (ss >> token)
            tokens.push_back(token);

        if (tokens.size() < 7)
            throw InvalidConfigException(filename, "Format property tidak valid");

        int id;
        try {
            id = std::stoi(tokens[0]);
        } catch (...) {
            throw InvalidConfigException(filename, "ID property bukan angka");
        }

        if (id < 1 || id > 40)
            throw InvalidConfigException(filename, "ID property out of range");

        int idx = id - 1;

        std::string jenis = tokens[3];

        if (jenis == "STREET") {
            properties[idx] = createStreetProperty(tokens);
        } else if (jenis == "RAILROAD") {
            properties[idx] = createRailroadProperty(tokens, rTable);
        } else if (jenis == "UTILITY") {
            properties[idx] = createUtilityProperty(tokens, uTable);
        } else {
            throw InvalidConfigException(filename, "Jenis property tidak dikenal");
        }
    }

    return properties;
}

// Load file railroad.txt  
std::map<int, int> ConfigLoader::loadRailroadRents(std::string filename) {
    std::map<int, int> rentTable;

    std::ifstream file(filename);
    if (!file.is_open())
        throw FileNotFoundException(filename);

    std::string line;
    if (!std::getline(file, line))
        throw InvalidConfigException(filename, "Header railroad.txt tidak valid");

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        int count, rent;

        if (!(ss >> count >> rent))
            throw InvalidConfigException(filename, "Format railroad.txt salah");

        rentTable[count] = rent;
    }

    return rentTable;
}

// Load file utility.txt
std::map<int, int> ConfigLoader::loadUtilityMultipliers(std::string filename) {
    std::map<int, int> multipliers;

    std::ifstream file(filename);
    if (!file.is_open())
        throw FileNotFoundException(filename);

    std::string line;
    if (!std::getline(file, line))
        throw InvalidConfigException(filename, "Header utility.txt tidak valid");

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        int count, mult;

        if (!(ss >> count >> mult))
            throw InvalidConfigException(filename, "Format utility.txt salah");

        multipliers[count] = mult;
    }

    return multipliers;
}

// Load file tax.txt
TaxConfig ConfigLoader::loadTaxConfig(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw FileNotFoundException(filename);

    std::string line;
    if (!std::getline(file, line))
        throw InvalidConfigException(filename, "Header tax.txt tidak valid");

    if (!std::getline(file, line))
        throw InvalidConfigException(filename, "Data tax.txt kosong");

    std::stringstream ss(line);

    int pphFlat, pphPercent, pbmFlat;

    if (!(ss >> pphFlat >> pphPercent >> pbmFlat))
        throw InvalidConfigException(filename, "Format tax.txt salah");

    return TaxConfig(pphFlat, pphPercent, pbmFlat);
}

// Load file special.txt
SpecialConfig ConfigLoader::loadSpecialConfig(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw FileNotFoundException(filename);

    std::string line;
    if (!std::getline(file, line))
        throw InvalidConfigException(filename, "Header special.txt tidak valid");

    if (!std::getline(file, line))
        throw InvalidConfigException(filename, "Data special.txt kosong");

    std::stringstream ss(line);

    int goSalary, jailFine;

    if (!(ss >> goSalary >> jailFine))
        throw InvalidConfigException(filename, "Format special.txt salah");

    return SpecialConfig(goSalary, jailFine);
}

// Load file misc.txt
MiscConfig ConfigLoader::loadMiscConfig(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw FileNotFoundException(filename);

    std::string line;
    if (!std::getline(file, line))
        throw InvalidConfigException(filename, "Header misc.txt tidak valid");

    if (!std::getline(file, line))
        throw InvalidConfigException(filename, "Data misc.txt kosong");

    std::stringstream ss(line);

    int maxTurn, initialBalance;

    if (!(ss >> maxTurn >> initialBalance))
        throw InvalidConfigException(filename, "Format misc.txt salah");

    return MiscConfig(maxTurn, initialBalance);
}

// Load file aksi.txt
std::map<int, ActionTileConfig> ConfigLoader::loadActionTiles(std::string filename) {
    std::map<int, ActionTileConfig> actionTiles;

    std::ifstream file(filename);
    if (!file.is_open())
        throw FileNotFoundException(filename);

    std::string line;
    if (!std::getline(file, line))
        throw InvalidConfigException(filename, "Header aksi.txt tidak valid");

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);

        int id;
        std::string kode, nama, jenis, warna;

        if (!(ss >> id >> kode >> nama >> jenis >> warna))
            throw InvalidConfigException(filename, "Format aksi.txt salah");

        if (id < 1 || id > 40)
            throw InvalidConfigException(filename, "ID aksi out of range");

        actionTiles[id - 1] = ActionTileConfig(id, kode, nama, jenis, warna);
    }

    return actionTiles;
}

// Convert data into Street Property objects
StreetProperty* ConfigLoader::createStreetProperty(std::vector<std::string> tokens) {
    // ID KODE NAMA JENIS WARNA HARGA_LAHAN NILAI_GADAI UPG_RUMAH UPG_HT
    // RENT_L0 RENT_L1 RENT_L2 RENT_L3 RENT_L4 RENT_L5
    std::string code = tokens[1];
    std::string name = tokens[2];
    std::string colorGroup = tokens[4];
    int purchasePrice = std::stoi(tokens[5]);
    int mortgageValue = std::stoi(tokens[6]);
    int houseBuildCost = std::stoi(tokens[7]);
    int hotelBuildCost = std::stoi(tokens[8]);

    if (tokens.size() < 15) {
        throw std::runtime_error("Format property STREET tidak lengkap untuk " + code);
    }

    std::vector<int> rentLevels;
    for (size_t i = 9; i <= 14; ++i) {
        rentLevels.push_back(std::stoi(tokens[i]));
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

TileColor ConfigLoader::colorGroupToTileColor(const std::string& group) {
    if (group == "COKLAT")     return TileColor::COKLAT;
    if (group == "BIRU_MUDA")  return TileColor::BIRU_MUDA;
    if (group == "MERAH_MUDA") return TileColor::MERAH_MUDA;
    if (group == "ORANGE")     return TileColor::ORANYE;
    if (group == "MERAH")      return TileColor::MERAH;
    if (group == "KUNING")     return TileColor::KUNING;
    if (group == "HIJAU")      return TileColor::HIJAU;
    if (group == "BIRU_TUA")   return TileColor::BIRU_TUA;
    return TileColor::DEFAULT;
}

Tile* ConfigLoader::createTileForIndex(int index,
        std::vector<Property*> properties, const GameConfig& config) {
    // Action/special tiles take priority (from aksi.txt)
    int id = index; // 0-based
    const auto& actionTiles = config.getActionTiles();
    if (actionTiles.find(id) != actionTiles.end()) {
        std::string kode = actionTiles.at(id).getKode();
        if (kode == "GO")  return new GoTile(id, config.getSpecial().getGoSalary());
        if (kode == "PEN") return new JailTile(id);
        if (kode == "BBP") return new FreeParkingTile(id);
        if (kode == "PPJ") return new GoToJailTile(id);
        if (kode == "DNU") return new CommunityChestTile(id);
        if (kode == "KSP") return new ChanceTile(id);
        if (kode == "FES") return new FestivalTile(id);
        if (kode == "PPH") return new IncomeTaxTile(id, config.getTax().getPphFlat(), config.getTax().getPphPercent());
        if (kode == "PBM") return new LuxuryTaxTile(id, config.getTax().getPbmFlat());
    }

    // Property tiles (street / railroad / utility)
    if (index >= 0 && index < (int)properties.size() && properties[index] != nullptr) {
        Property* prop = properties[index];
        if (prop->isStreet()) {
            auto* sp = static_cast<StreetProperty*>(prop);
            TileColor color = colorGroupToTileColor(sp->getColorGroup());
            return new StreetTile(id, sp->getCode(), sp->getName(), color, sp);
        }
        if (prop->isRailroad()) {
            auto* rp = static_cast<RailroadProperty*>(prop);
            return new RailroadTile(id, rp->getCode(), rp->getName(), rp);
        }
        if (prop->isUtility()) {
            auto* up = static_cast<UtilityProperty*>(prop);
            return new UtilityTile(id, up->getCode(), up->getName(), up);
        }
    }

    return nullptr;
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