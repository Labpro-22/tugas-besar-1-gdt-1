#include "core/CommandProcessor.hpp"

namespace {
int buildingLevel(const StreetProperty* property) {
    return static_cast<int>(property->getBuildingState());
}

std::string buildingStateLabel(const StreetProperty* property) {
    if (property == nullptr) {
        return "";
    }

    switch (property->getBuildingState()) {
        case BuildingState::HOUSE_1: return "1 rumah";
        case BuildingState::HOUSE_2: return "2 rumah";
        case BuildingState::HOUSE_3: return "3 rumah";
        case BuildingState::HOUSE_4: return "4 rumah";
        case BuildingState::HOTEL:   return "Hotel";
        default:                     return "";
    }
}
}

CommandProcessor::CommandProcessor(GameEngine* engine, Game* game, TurnManager* turn, DiceManager* dice, IGUI* gui)
    : engine(engine), game(game), turn(turn), dice(dice), gui(gui) {}

std::vector<std::string> CommandProcessor::tokenize(const std::string& cmd) const {
    std::vector<std::string> out;
    std::istringstream iss(cmd);
    std::string tok;
    while (iss >> tok) out.push_back(tok);
    return out;
}

std::string CommandProcessor::normalize(const std::string& s) const {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(), [](unsigned char c) { return std::toupper(c); });
    return r;
}

bool CommandProcessor::isAwaitingBonusRoll(Player* player) const {
    return player != nullptr &&
           player->hasRolled() &&
           !player->hasPendingFestival() &&
           turn->canRoll(player);
}

bool CommandProcessor::isAllowedDuringBonusRoll(const std::string& cmd) const {
    return cmd == "LEMPAR_DADU" ||
           cmd == "ATUR_DADU" ||
           cmd == "HELP" ||
           cmd == "CETAK_PAPAN" ||
           cmd == "CETAK_AKTA" ||
           cmd == "CETAK_DEED" ||
           cmd == "CETAK_PROPERTI" ||
           cmd == "CETAK_LOG" ||
           cmd == "SIMPAN" ||
           cmd == "MUAT" ||
           cmd == "EXIT" ||
           cmd == "CLOSE" ||
           cmd == "CLOSE GAME";
}

CommandResult CommandProcessor::process(const std::string& command, Player* player) {
    if (command.empty() || command == "NULL") return CommandResult::CONTINUE;

    auto tokens = tokenize(command);
    if (tokens.empty()) return CommandResult::CONTINUE;

    std::string cmd = normalize(tokens[0]);

    if (isAwaitingBonusRoll(player) && !isAllowedDuringBonusRoll(cmd)) {
        gui->showMessage("Kamu masih memiliki lemparan bonus karena double.");
        gui->showMessage("Gunakan LEMPAR_DADU atau ATUR_DADU <x> <y> terlebih dahulu.");
        return CommandResult::INVALID;
    }

    try {
        if (cmd == "LEMPAR_DADU") {
            return handleRoll(player, false, 0, 0);
        }
        if (cmd == "ATUR_DADU") {
            if (tokens.size() < 3) {
                gui->showMessage("Format: ATUR_DADU <d1> <d2>");
                return CommandResult::INVALID;
            }
            int d1 = std::stoi(tokens[1]);
            int d2 = std::stoi(tokens[2]);
            if (d1 < 1 || d1 > 6 || d2 < 1 || d2 > 6) {
                gui->showMessage("Nilai dadu harus berada pada rentang 1 sampai 6.");
                return CommandResult::INVALID;
            }
            return handleRoll(player, true, d1, d2);
        }
        if (cmd == "CETAK_PAPAN")     return handlePrintBoard();
        if (cmd == "CETAK_DEED" || cmd == "CETAK_AKTA") {
            return handlePrintDeed(tokens.size() >= 2 ? tokens[1] : "");
        }
        if (cmd == "CETAK_PROPERTI")  return handlePrintProperty(player);
        if (cmd == "GADAI") {
            if (tokens.size() < 2) { gui->showMessage("Format: GADAI <kode>"); return CommandResult::INVALID; }
            return handleMortgage(player, tokens[1]);
        }
        if (cmd == "TEBUS") {
            if (tokens.size() < 2) { gui->showMessage("Format: TEBUS <kode>"); return CommandResult::INVALID; }
            return handleRedeem(player, tokens[1]);
        }
        if (cmd == "BANGUN") {
            if (tokens.size() < 2) { gui->showMessage("Format: BANGUN <kode>"); return CommandResult::INVALID; }
            return handleBuild(player, tokens[1]);
        }
        if (cmd == "GUNAKAN_KEMAMPUAN") {
            if (tokens.size() < 2) { gui->showMessage("Format: GUNAKAN_KEMAMPUAN <nomor_kartu>"); return CommandResult::INVALID; }
            return handleUseSkill(player, std::stoi(tokens[1]) - 1);
        }
        if (cmd == "CETAK_KARTU") return handleShowCards(player);
        if (cmd == "BUANG_KARTU") {
            if (tokens.size() < 2) { gui->showMessage("Format: BUANG_KARTU <nomor_kartu>"); return CommandResult::INVALID; }
            return handleDropCard(player, std::stoi(tokens[1]) - 1);
        }
        if (cmd == "CETAK_LOG") {
            int n = (tokens.size() >= 2) ? std::stoi(tokens[1]) : 0;
            return handlePrintLog(n);
        }
        if (cmd == "HELP") {
            return handleHelp(player);
        }
        if (cmd == "SIMPAN") {
            if (tokens.size() < 2) { gui->showMessage("Format: SIMPAN <file>"); return CommandResult::INVALID; }
            return handleSave(player, tokens[1]);
        }
        if (cmd == "MUAT") {
            if (tokens.size() < 2) { gui->showMessage("Format: MUAT <nama_save>"); return CommandResult::INVALID; }
            return handleLoad(tokens[1]);
        }
        if (cmd == "FESTIVAL") {
            if (tokens.size() < 2) { gui->showMessage("Format: FESTIVAL <kode>"); return CommandResult::INVALID; }
            return handleFestival(player, tokens[1]);
        }
        if (cmd == "AKHIRI_GILIRAN") return handleEndTurn(player);
        if (cmd == "EXIT" || cmd == "CLOSE" || cmd == "CLOSE GAME") {
            game->setGameOver(true);
            return CommandResult::GAME_OVER;
        }

        gui->showMessage("Perintah tidak dikenal: " + cmd);
        gui->showMessage("Ketik HELP untuk melihat daftar perintah yang tersedia.");
        return CommandResult::INVALID;
    } catch (const std::exception& e) {
        gui->showMessage("Perintah tidak dapat diproses.");
        gui->showMessage(std::string("Detail: ") + e.what());
        return CommandResult::INVALID;
    }
}

CommandResult CommandProcessor::handleRoll(Player* player, bool manual, int d1, int d2) {
    return engine->resolveRoll(player, manual, d1, d2, false);
}

CommandResult CommandProcessor::handlePrintBoard() {
    gui->renderBoard(*game);
    return CommandResult::CONTINUE;
}

CommandResult CommandProcessor::handlePrintDeed(const std::string& code) {
    std::string targetCode = code;
    if (targetCode.empty()) {
        gui->showInputPrompt("Masukkan kode petak:");
        targetCode = gui->getCommand();
        if (targetCode.empty() || targetCode == "NULL") {
            gui->showMessage("Kode petak tidak boleh kosong.");
            return CommandResult::INVALID;
        }
    }

    targetCode = normalize(targetCode);
    Tile* tile = nullptr;
    try {
        tile = game->getBoard()->getTile(targetCode);
    } catch (...) {
        gui->showMessage("Petak \"" + targetCode + "\" tidak ditemukan atau bukan properti.");
        return CommandResult::INVALID;
    }
    auto* pt = dynamic_cast<PropertyTile*>(tile);
    if (!pt || !pt->getProperty()) {
        gui->showMessage("Petak \"" + targetCode + "\" tidak ditemukan atau bukan properti.");
        return CommandResult::INVALID;
    }
    gui->renderProperty(*pt->getProperty());
    return CommandResult::CONTINUE;
}

CommandResult CommandProcessor::handlePrintProperty(Player* player) {
    const auto& owned = player->getOwnedProperties();
    if (owned.empty()) {
        gui->showMessage("Kamu belum memiliki properti apapun.");
        return CommandResult::CONTINUE;
    }
    gui->renderOwnedProperties(*player);
    return CommandResult::CONTINUE;
}

CommandResult CommandProcessor::handleMortgage(Player* player, const std::string& code) {
    Board* board = game->getBoard();
    if (board == nullptr) {
        gui->showMessage("Papan permainan belum tersedia.");
        return CommandResult::INVALID;
    }

    std::string targetCode = normalize(code);
    Tile* tile = board->getTile(targetCode);
    auto* pt = dynamic_cast<PropertyTile*>(tile);
    Property* prop = pt ? pt->getProperty() : nullptr;
    if (!prop || prop->getOwner() != player) {
        gui->showMessage("Kamu tidak memiliki properti " + targetCode + ".");
        return CommandResult::INVALID;
    }
    if (prop->isMortgaged()) {
        gui->showMessage("Properti sudah digadai.");
        return CommandResult::INVALID;
    }

    if (auto* sp = dynamic_cast<StreetProperty*>(prop)) {
        std::vector<StreetProperty*> group = board->getStreetGroup(sp->getColorGroup());
        std::vector<StreetProperty*> builtStreets;

        for (StreetProperty* street : group) {
            if (street != nullptr &&
                street->getOwner() == player &&
                street->getBuildingState() != BuildingState::NONE) {
                builtStreets.push_back(street);
            }
        }

        if (!builtStreets.empty()) {
            gui->showMessage(prop->getName() + " tidak dapat digadaikan.");
            gui->showMessage("Masih terdapat bangunan di color group [" + sp->getColorGroup() + "].");
            gui->showMessage("Bangunan harus dijual terlebih dahulu.");
            gui->showMessage("Daftar bangunan di color group [" + sp->getColorGroup() + "]:");

            for (size_t i = 0; i < builtStreets.size(); ++i) {
                StreetProperty* street = builtStreets[i];
                const int saleValue = street->sellBuildingValue();
                gui->showMessage(std::to_string(i + 1) + ". " +
                                 street->getName() + " (" + street->getCode() + ") - " +
                                 buildingStateLabel(street) +
                                 " -> Nilai jual bangunan: M" + std::to_string(saleValue));
            }

            std::string answer;
            while (true) {
                answer = normalize(waitInput("Jual semua bangunan color group [" +
                                             sp->getColorGroup() + "]? (y/n):"));
                if (answer == "Y" || answer == "YA" || answer == "YES" ||
                    answer == "N" || answer == "NO" || answer == "TIDAK") {
                    break;
                }
                gui->showMessage("Masukan tidak valid. Gunakan y/n.");
            }

            if (answer == "N" || answer == "NO" || answer == "TIDAK") {
                gui->showMessage("Penggadaian dibatalkan.");
                return CommandResult::INVALID;
            }

            for (StreetProperty* street : builtStreets) {
                const int saleValue = street->sellBuildingValue();
                street->clearBuildings();
                player->addMoney(saleValue);
                gui->showMessage("Bangunan " + street->getName() + " terjual. Kamu menerima M" +
                                 std::to_string(saleValue) + ".");
                if (engine->logger != nullptr) {
                    engine->logger->log(game->getCurrentTurn(), player->getUsername(),
                                        "JUAL_BANGUNAN",
                                        "Jual bangunan di " + street->getName() + " (" +
                                        street->getCode() + ") senilai M" +
                                        std::to_string(saleValue));
                }
            }

            gui->showMessage("Uang kamu sekarang: M" + std::to_string(player->getBalance()));

            while (true) {
                answer = normalize(waitInput("Lanjut menggadaikan " + prop->getName() + "? (y/n):"));
                if (answer == "Y" || answer == "YA" || answer == "YES" ||
                    answer == "N" || answer == "NO" || answer == "TIDAK") {
                    break;
                }
                gui->showMessage("Masukan tidak valid. Gunakan y/n.");
            }

            if (answer == "N" || answer == "NO" || answer == "TIDAK") {
                gui->showMessage("Penggadaian dibatalkan.");
                return CommandResult::INVALID;
            }
        }
    }

    int value = prop->getMortgageValue();
    prop->setStatus(PropertyStatus::MORTGAGED);
    player->addMoney(value);
    gui->showMessage(prop->getName() + " berhasil digadaikan.");
    gui->showMessage("Kamu menerima M" + std::to_string(value) + " dari Bank.");
    gui->showMessage("Uang kamu sekarang: M" + std::to_string(player->getBalance()));
    gui->showMessage("Catatan: Sewa tidak dapat dipungut dari properti yang digadaikan.");
    if (engine->logger != nullptr) {
        engine->logger->log(game->getCurrentTurn(), player->getUsername(),
                            "GADAI",
                            prop->getName() + " (" + prop->getCode() + ") senilai M" +
                            std::to_string(value));
    }
    return CommandResult::CONTINUE;
}

CommandResult CommandProcessor::handleRedeem(Player* player, const std::string& code) {
    std::string targetCode = normalize(code);
    Tile* tile = game->getBoard()->getTile(targetCode);
    auto* pt = dynamic_cast<PropertyTile*>(tile);
    Property* prop = pt ? pt->getProperty() : nullptr;
    if (!prop || prop->getOwner() != player) {
        gui->showMessage("Kamu tidak memiliki properti " + targetCode + ".");
        return CommandResult::INVALID;
    }
    if (!prop->isMortgaged()) {
        gui->showMessage("Properti tidak sedang digadai.");
        return CommandResult::INVALID;
    }
    int cost = prop->getPurchasePrice();
    if (!player->canAfford(cost)) {
        gui->showMessage("Uang kamu tidak cukup untuk menebus " + prop->getName() + ".");
        gui->showMessage("Harga tebus: M" + std::to_string(cost) +
                         " | Uang kamu: M" + std::to_string(player->getBalance()));
        return CommandResult::INVALID;
    }
    player->deductMoney(cost);
    prop->setStatus(PropertyStatus::OWNED);
    gui->showMessage(prop->getName() + " berhasil ditebus!");
    gui->showMessage("Kamu membayar M" + std::to_string(cost) + " ke Bank.");
    gui->showMessage("Uang kamu sekarang: M" + std::to_string(player->getBalance()));
    if (engine->logger != nullptr) {
        engine->logger->log(game->getCurrentTurn(), player->getUsername(),
                            "TEBUS",
                            prop->getName() + " (" + prop->getCode() + ") seharga M" +
                            std::to_string(cost));
    }
    return CommandResult::CONTINUE;
}

CommandResult CommandProcessor::handleBuild(Player* player, const std::string& code) {
    Tile* tile = game->getBoard()->getTile(code);
    auto* pt = dynamic_cast<PropertyTile*>(tile);
    auto* sp = pt ? dynamic_cast<StreetProperty*>(pt->getProperty()) : nullptr;
    if (!sp || sp->getOwner() != player) {
        gui->showMessage("Kamu tidak memiliki properti street " + code + ".");
        return CommandResult::INVALID;
    }
    if (sp->isMortgaged()) {
        gui->showMessage("Tidak dapat membangun di properti yang digadai.");
        return CommandResult::INVALID;
    }

    Board* board = game->getBoard();
    if (board == nullptr) {
        gui->showMessage("Papan permainan belum tersedia.");
        return CommandResult::INVALID;
    }

    std::vector<StreetProperty*> group = board->getStreetGroup(sp->getColorGroup());
    if (group.empty()) {
        gui->showMessage("Color group untuk properti ini tidak ditemukan.");
        return CommandResult::INVALID;
    }

    bool ownsFullGroup = true;
    for (StreetProperty* street : group) {
        if (street == nullptr || street->getOwner() != player) {
            ownsFullGroup = false;
            break;
        }
    }

    if (!ownsFullGroup) {
        gui->showMessage("Butuh satu set warna lengkap untuk membangun.");
        return CommandResult::INVALID;
    }

    int minLevel = buildingLevel(group.front());
    for (StreetProperty* street : group) {
        const int level = buildingLevel(street);
        if (level < minLevel) minLevel = level;
    }

    const int targetLevel = buildingLevel(sp);

    if (sp->getBuildingState() == BuildingState::HOTEL) {
        gui->showMessage("Properti ini sudah memiliki Hotel dan tidak dapat dibangun lagi.");
        return CommandResult::INVALID;
    }

    if (targetLevel < static_cast<int>(BuildingState::HOUSE_4)) {
        if (targetLevel != minLevel) {
            gui->showMessage("Pembangunan rumah harus dilakukan merata dalam color group yang sama.");
            return CommandResult::INVALID;
        }

        const int cost = sp->getHouseBuildCost();
        if (!player->canAfford(cost)) {
            gui->showMessage("Uang kamu tidak cukup untuk membangun rumah. Biaya: M" + std::to_string(cost));
            return CommandResult::INVALID;
        }

        if (!sp->buildHouse()) {
            gui->showMessage("Tidak dapat membangun rumah di " + code);
            return CommandResult::INVALID;
        }

        player->deductMoney(cost);
        gui->showMessage("Kamu membangun 1 rumah di " + code + ". Biaya: M" + std::to_string(cost));
        gui->showMessage("Uang tersisa: M" + std::to_string(player->getBalance()));
        if (engine->logger != nullptr) {
            engine->logger->log(game->getCurrentTurn(), player->getUsername(),
                                "BANGUN",
                                "Bangun rumah di " + sp->getName() + " (" + sp->getCode() +
                                ") seharga M" + std::to_string(cost));
        }
        return CommandResult::CONTINUE;
    }

    bool allReadyForHotel = true;
    for (StreetProperty* street : group) {
        if (buildingLevel(street) < static_cast<int>(BuildingState::HOUSE_4)) {
            allReadyForHotel = false;
            break;
        }
    }

    if (!allReadyForHotel) {
        gui->showMessage("Semua properti dalam color group harus memiliki 4 rumah sebelum upgrade ke Hotel.");
        return CommandResult::INVALID;
    }

    const int cost = sp->getHotelBuildCost();
    if (!player->canAfford(cost)) {
        gui->showMessage("Uang kamu tidak cukup untuk upgrade ke Hotel. Biaya: M" + std::to_string(cost));
        return CommandResult::INVALID;
    }

    if (!sp->buildHotel()) {
        gui->showMessage("Tidak dapat upgrade ke Hotel di " + code);
        return CommandResult::INVALID;
    }

    player->deductMoney(cost);
    gui->showMessage("Kamu upgrade " + code + " menjadi Hotel. Biaya: M" + std::to_string(cost));
    gui->showMessage("Uang tersisa: M" + std::to_string(player->getBalance()));
    if (engine->logger != nullptr) {
        engine->logger->log(game->getCurrentTurn(), player->getUsername(),
                            "BANGUN",
                            "Bangun hotel di " + sp->getName() + " (" + sp->getCode() +
                            ") seharga M" + std::to_string(cost));
    }
    return CommandResult::CONTINUE;
}

CommandResult CommandProcessor::handlePrintLog(int nLast) {
    auto* logger = engine->logger;
    std::vector<LogEntry> entries = (nLast > 0)
        ? logger->getRecentLog(nLast)
        : logger->getFullLog();
    const std::string title = (nLast > 0)
        ? "=== Log Transaksi (" + std::to_string(nLast) + " Terakhir) ==="
        : "=== Log Transaksi Penuh ===";
    gui->renderLog(entries, title);
    return CommandResult::CONTINUE;
}

CommandResult CommandProcessor::handleSave(Player* player, const std::string& file) {
    if (engine->saveLoadManager == nullptr) {
        gui->showMessage("Fitur simpan permainan belum siap digunakan.");
        return CommandResult::INVALID;
    }

    if (player == nullptr || turn == nullptr) {
        gui->showMessage("Permainan belum berada pada giliran pemain yang valid untuk disimpan.");
        return CommandResult::INVALID;
    }

    if (turn->hasActedThisTurn() || player->hasRolled() || player->hasUsedSkill() ||
        player->hasPendingFestival()) {
        gui->showMessage("SIMPAN hanya dapat dilakukan di awal giliran, sebelum aksi apapun dijalankan.");
        return CommandResult::INVALID;
    }

    if (engine->saveLoadManager->saveTargetExists(file)) {
        std::string answer;
        while (true) {
            answer = normalize(waitInput("Data simpan \"" + file + "\" sudah ada. Timpa data lama? (y/n):"));
            if (answer == "Y" || answer == "YA" || answer == "YES" ||
                answer == "N" || answer == "NO" || answer == "TIDAK") {
                break;
            }
            gui->showMessage("Masukan tidak valid. Gunakan y/n.");
        }

        if (answer == "N" || answer == "NO" || answer == "TIDAK") {
            gui->showMessage("Simpan dibatalkan.");
            return CommandResult::INVALID;
        }
    }

    if (engine->saveLoadManager->save(file)) {
        if (engine->logger != nullptr) {
            engine->logger->log(game->getCurrentTurn(),
                                player ? player->getUsername() : "SISTEM",
                                "SIMPAN", "Game disimpan ke " + file);
            if (!engine->saveLoadManager->saveLogSnapshot(file)) {
                gui->showMessage("Peringatan: game tersimpan, tetapi log save gagal diperbarui.");
            }
        }
        gui->showMessage("Permainan berhasil disimpan ke " + file + ".");
        return CommandResult::CONTINUE;
    }
    gui->showMessage("Gagal menyimpan permainan ke " + file + ".");
    return CommandResult::INVALID;
}

CommandResult CommandProcessor::handleLoad(const std::string& file) {
    (void)file;
    gui->showMessage("MUAT hanya dapat dilakukan sebelum permainan dimulai, dari menu utama.");
    return CommandResult::INVALID;
}

CommandResult CommandProcessor::handleFestival(Player* player, const std::string& code) {
    if (!player->hasPendingFestival()) {
        gui->showMessage("Tidak ada efek Festival yang menunggu pilihan properti.");
        return CommandResult::INVALID;
    }
    Property* target = nullptr;
    for (Property* p : player->getOwnedProperties()) {
        if (p->getCode() == code) { target = p; break; }
    }
    if (target == nullptr) {
        gui->showMessage("Properti " + code + " tidak ditemukan atau bukan milikmu.");
        return CommandResult::INVALID;
    }
    if (target->isMortgaged()) {
        gui->showMessage("Properti " + code + " sedang digadai dan tidak dapat dipilih untuk Festival.");
        return CommandResult::INVALID;
    }
    int rentBefore = target->calculateRent(0);
    int multBefore = target->getFestivalMultiplier();
    bool wasActive = target->getFestivalDuration() > 0;
    target->activateFestival();
    player->setPendingFestival(false);
    int rentAfter  = target->calculateRent(0);
    int multAfter  = target->getFestivalMultiplier();
    int dur        = target->getFestivalDuration();
    if (engine->logger != nullptr) {
        std::string logDetail = target->getName() + " (" + target->getCode() + "): ";
        if (!wasActive) {
            logDetail += "sewa M" + std::to_string(rentBefore) + " -> M" +
                         std::to_string(rentAfter) + ", durasi " + std::to_string(dur) + " giliran";
        } else if (multAfter > multBefore) {
            logDetail += "Efek diperkuat! sewa M" + std::to_string(rentBefore) +
                         " -> M" + std::to_string(rentAfter) +
                         ", durasi di-reset " + std::to_string(dur) + " giliran";
        } else {
            logDetail += "Efek sudah maksimum (x" + std::to_string(multAfter) +
                         "), durasi di-reset " + std::to_string(dur) + " giliran";
        }
        engine->logger->log(game->getCurrentTurn(), player->getUsername(), "FESTIVAL", logDetail);
    }
    if (!wasActive) {
        gui->showMessage("Festival aktif di " + target->getName() + ".");
        gui->showMessage("Sewa properti ini berlipat selama " + std::to_string(dur) + " giliran.");
    } else if (multAfter > multBefore) {
        gui->showMessage("Efek Festival di " + target->getName() + " diperkuat.");
        gui->showMessage("Sewa sekarang M" + std::to_string(rentAfter) +
                         " dan durasi di-reset menjadi " + std::to_string(dur) + " giliran.");
    } else {
        gui->showMessage("Efek Festival di " + target->getName() +
                         " sudah berada di level maksimum (x" + std::to_string(multAfter) + ").");
        gui->showMessage("Durasi di-reset menjadi " + std::to_string(dur) + " giliran.");
    }
    if (isAwaitingBonusRoll(player)) {
        gui->showMessage("Karena lemparan sebelumnya double, lanjutkan dengan LEMPAR_DADU atau ATUR_DADU <x> <y>.");
    } else {
        gui->showMessage("Gunakan perintah lain atau AKHIRI_GILIRAN.");
    }
    return CommandResult::CONTINUE;
}

CommandResult CommandProcessor::handleEndTurn(Player* player) {
    if (!player->hasRolled()) {
        gui->showMessage("Lempar dadu dulu sebelum mengakhiri giliran.");
        return CommandResult::INVALID;
    }
    if (isAwaitingBonusRoll(player)) {
        gui->showMessage("Kamu masih memiliki lemparan bonus karena double.");
        gui->showMessage("Selesaikan dengan LEMPAR_DADU atau ATUR_DADU <x> <y> terlebih dahulu.");
        return CommandResult::INVALID;
    }
    if (player->hasPendingFestival()) {
        gui->showMessage("Pilih dulu properti untuk Festival dengan FESTIVAL <kode>.");
        return CommandResult::INVALID;
    }
    turn->endTurn(player);
    return CommandResult::END_TURN;
}

CommandResult CommandProcessor::handleHelp(Player* player) {
    gui->showMessage("Daftar perintah umum:");
    gui->showMessage("CETAK_PAPAN | CETAK_AKTA <kode> | CETAK_PROPERTI | CETAK_LOG [n]");

    if (player == nullptr) {
        gui->showMessage("LEMPAR_DADU | ATUR_DADU <x> <y> | AKHIRI_GILIRAN");
        return CommandResult::CONTINUE;
    }

    if (!player->hasRolled()) {
        gui->showMessage("Sebelum lempar dadu: LEMPAR_DADU | ATUR_DADU <x> <y> | GUNAKAN_KEMAMPUAN <nomor_kartu> | SIMPAN <file>");
    } else if (player->hasPendingFestival()) {
        gui->showMessage("Saat aksi Festival: FESTIVAL <kode_properti> | CETAK_PAPAN | CETAK_PROPERTI | CETAK_LOG [n]");
        if (player->getConsecutiveDoubles() > 0) {
            gui->showMessage("Setelah Festival selesai, lanjutkan dengan LEMPAR_DADU atau ATUR_DADU <x> <y>.");
        }
    } else if (isAwaitingBonusRoll(player)) {
        gui->showMessage("Saat menunggu lemparan bonus: LEMPAR_DADU | ATUR_DADU <x> <y> | CETAK_PAPAN | CETAK_AKTA <kode> | CETAK_PROPERTI | CETAK_LOG [n] | SIMPAN <file>");
    } else {
        gui->showMessage("Setelah lempar dadu: GADAI <kode> | TEBUS <kode> | BANGUN <kode> | CETAK_PAPAN | CETAK_PROPERTI | AKHIRI_GILIRAN");
    }

    gui->showMessage("Kartu kemampuan: CETAK_KARTU | GUNAKAN_KEMAMPUAN <nomor_kartu> | BUANG_KARTU <nomor_kartu>");
    gui->showMessage("Simpan/muat: SIMPAN <nama_save> (awal giliran) | MUAT dari menu utama");
    gui->showMessage("Keluar permainan: EXIT");
    return CommandResult::CONTINUE;
}

// ── Skill card helpers ────────────────────────────────────────────────────────

std::string CommandProcessor::waitInput(const std::string& prompt) const {
    gui->showInputPrompt(prompt);
    while (true) {
        gui->update();
        gui->display();
        std::string s = gui->getCommand();
        if (!s.empty() && s != "NULL") return s;
    }
}

CommandResult CommandProcessor::handleShowCards(Player* player) {
    const auto& hand = player->getHandCards();
    if (hand.empty()) {
        gui->showMessage("Kamu tidak memiliki kartu kemampuan di tangan.");
        return CommandResult::CONTINUE;
    }
    gui->showMessage("Kartu kemampuan di tanganmu:");
    for (int i = 0; i < static_cast<int>(hand.size()); ++i) {
        SkillCard* c = hand[i];
        gui->showMessage("[" + std::to_string(i + 1) + "] " + c->getCardName() +
                         " — " + c->getDescription());
    }
    return CommandResult::CONTINUE;
}

CommandResult CommandProcessor::handleDropCard(Player* player, int index) {
    const auto& hand = player->getHandCards();
    if (index < 0 || index >= static_cast<int>(hand.size())) {
        gui->showMessage("Nomor kartu tidak valid.");
        return CommandResult::INVALID;
    }
    SkillCard* card = hand[index];
    player->removeCard(card);
    if (game->getSkillDeck() != nullptr) game->getSkillDeck()->discard(card);
    if (engine->logger != nullptr) {
        engine->logger->log(game->getCurrentTurn(), player->getUsername(),
                            "BUANG", "Buang " + card->getCardName());
    }
    gui->showMessage("Kartu " + card->getCardName() + " dibuang.");
    return CommandResult::CONTINUE;
}

// ── Apply effects per card type ───────────────────────────────────────────────

void CommandProcessor::applyMoveCard(Player* player, MoveCard* card) {
    int steps = card->getSteps();
    Board* board = game->getBoard();
    if (board == nullptr) return;
    if (board->passesGo(player->getPosition(), steps)) {
        player->addMoney(game->getGoSalary());
        gui->showMessage("Melewati GO! Terima M" + std::to_string(game->getGoSalary()) + ".");
    }
    int newPos = board->getNextIndex(player->getPosition(), steps);
    player->setPosition(newPos);
    Tile* landed = board->getTile(newPos);
    std::string tileName = landed ? landed->getName() : "?";
    gui->showMessage("MoveCard: maju " + std::to_string(steps) + " petak -> mendarat di " + tileName + ".");
    if (engine->logger != nullptr) {
        engine->logger->log(game->getCurrentTurn(), player->getUsername(),
                            "KARTU", "Pakai MoveCard maju " + std::to_string(steps) +
                            " petak -> " + tileName);
    }
    if (landed) engine->handleTileLandingPublic(player, landed);
}

void CommandProcessor::applyTeleportCard(Player* player, TeleportCard* /*card*/) {
    Board* board = game->getBoard();
    if (board == nullptr) return;
    gui->showMessage("TeleportCard: kamu dapat berpindah ke petak manapun.");
    gui->showMessage("Masukkan kode petak tujuan.");
    while (true) {
        std::string code = waitInput("Kode petak tujuan:");
        std::transform(code.begin(), code.end(), code.begin(),
                       [](unsigned char c){ return std::toupper(c); });
        Tile* target = nullptr;
        try {
            target = board->getTile(code);
        } catch (...) {
            gui->showMessage("Kode petak tidak ditemukan: " + code + ". Coba lagi.");
            continue;
        }
        if (board->passesGo(player->getPosition(), 0) ||
            target->getIndex() < player->getPosition()) {
            // hanya berikan GO salary jika benar-benar melewati GO (index menurun)
        }
        player->setPosition(target->getIndex());
        gui->showMessage("Teleport ke " + target->getName() + " (" + code + ").");
        if (engine->logger != nullptr) {
            engine->logger->log(game->getCurrentTurn(), player->getUsername(),
                                "KARTU", "Pakai TeleportCard -> pindah ke " +
                                target->getName() + " (" + code + ")");
        }
        engine->handleTileLandingPublic(player, target);
        break;
    }
}

void CommandProcessor::applyDiscountCard(Player* player, DiscountCard* card) {
    // Efek DiscountCard: diskon pembelian properti selama 1 giliran.
    // Diskon disimpan di player sebagai pending discount yang dicek saat beli properti.
    player->setPendingDiscount(card->getDiscount());
    gui->showMessage("DiscountCard aktif: diskon " + std::to_string(card->getDiscount()) +
                     "% untuk pembelian properti giliran ini.");
    if (engine->logger != nullptr) {
        engine->logger->log(game->getCurrentTurn(), player->getUsername(),
                            "KARTU", "Pakai DiscountCard diskon " +
                            std::to_string(card->getDiscount()) + "%");
    }
}

void CommandProcessor::applyShieldCard(Player* player, ShieldCard* /*card*/) {
    turn->setShieldActive(true);
    gui->showMessage("ShieldCard aktif: kamu kebal dari tagihan sewa dan sanksi giliran ini.");
    if (engine->logger != nullptr) {
        engine->logger->log(game->getCurrentTurn(), player->getUsername(),
                            "KARTU", "Pakai ShieldCard -> kebal sewa & sanksi");
    }
}

void CommandProcessor::applyLassoCard(Player* player, LassoCard* /*card*/) {
    const auto& activePlayers = game->getActivePlayers();
    // Cari pemain lawan yang berada DI DEPAN posisi player
    std::vector<Player*> candidates;
    int myPos = player->getPosition();
    int boardSize = (game->getBoard() ? game->getBoard()->getSize() : 40);
    for (Player* other : activePlayers) {
        if (other == player || other->isJailed()) continue;
        int diff = (other->getPosition() - myPos + boardSize) % boardSize;
        if (diff > 0) candidates.push_back(other);
    }
    if (candidates.empty()) {
        gui->showMessage("LassoCard: tidak ada pemain lawan yang berada di depanmu.");
        if (engine->logger != nullptr) {
            engine->logger->log(game->getCurrentTurn(), player->getUsername(),
                                "KARTU", "Pakai LassoCard -> tidak ada target");
        }
        return;
    }
    // Sort by smallest distance (paling dekat di depan)
    std::sort(candidates.begin(), candidates.end(), [&](Player* a, Player* b){
        int da = (a->getPosition() - myPos + boardSize) % boardSize;
        int db = (b->getPosition() - myPos + boardSize) % boardSize;
        return da < db;
    });
    gui->showMessage("LassoCard: pilih pemain yang akan ditarik.");
    for (int i = 0; i < static_cast<int>(candidates.size()); ++i) {
        gui->showMessage("[" + std::to_string(i + 1) + "] " + candidates[i]->getUsername() +
                         " (petak " + std::to_string(candidates[i]->getPosition()) + ")");
    }
    int choice = -1;
    while (choice < 0 || choice >= static_cast<int>(candidates.size())) {
        std::string in = waitInput("Pilih nomor pemain:");
        try { choice = std::stoi(in) - 1; } catch (...) { choice = -1; }
        if (choice < 0 || choice >= static_cast<int>(candidates.size()))
            gui->showMessage("Nomor pemain tidak valid.");
    }
    Player* target = candidates[choice];
    target->setPosition(myPos);
    gui->showMessage("LassoCard: " + target->getUsername() + " ditarik ke petakmu (" +
                     std::to_string(myPos) + ").");
    if (engine->logger != nullptr) {
        engine->logger->log(game->getCurrentTurn(), player->getUsername(),
                            "KARTU", "Pakai LassoCard -> tarik " + target->getUsername() +
                            " ke petak " + std::to_string(myPos));
    }
}

void CommandProcessor::applyDemolitionCard(Player* player, DemolitionCard* /*card*/) {
    // Kumpulkan semua properti lawan yang punya bangunan
    std::vector<std::pair<Player*, Property*>> targets;
    for (Player* other : game->getActivePlayers()) {
        if (other == player) continue;
        for (Property* prop : other->getOwnedProperties()) {
            auto* sp = dynamic_cast<StreetProperty*>(prop);
            if (sp && sp->getBuildingState() != BuildingState::NONE) {
                targets.push_back({other, prop});
            }
        }
    }
    if (targets.empty()) {
        gui->showMessage("DemolitionCard: tidak ada bangunan milik lawan yang bisa dihancurkan.");
        if (engine->logger != nullptr) {
            engine->logger->log(game->getCurrentTurn(), player->getUsername(),
                                "KARTU", "Pakai DemolitionCard -> tidak ada target");
        }
        return;
    }
    gui->showMessage("DemolitionCard: pilih properti lawan yang akan dihancurkan.");
    for (int i = 0; i < static_cast<int>(targets.size()); ++i) {
        auto* sp = dynamic_cast<StreetProperty*>(targets[i].second);
        std::string bldg;
        switch (sp->getBuildingState()) {
            case BuildingState::HOUSE_1: bldg = "1 rumah"; break;
            case BuildingState::HOUSE_2: bldg = "2 rumah"; break;
            case BuildingState::HOUSE_3: bldg = "3 rumah"; break;
            case BuildingState::HOUSE_4: bldg = "4 rumah"; break;
            case BuildingState::HOTEL:   bldg = "Hotel"; break;
            default: bldg = "-"; break;
        }
        gui->showMessage("[" + std::to_string(i + 1) + "] " +
                         targets[i].first->getUsername() + " — " +
                         sp->getName() + " (" + sp->getCode() + ") " + bldg);
    }
    int choice = -1;
    while (choice < 0 || choice >= static_cast<int>(targets.size())) {
        std::string in = waitInput("Pilih nomor properti:");
        try { choice = std::stoi(in) - 1; } catch (...) { choice = -1; }
        if (choice < 0 || choice >= static_cast<int>(targets.size()))
            gui->showMessage("Nomor properti tidak valid.");
    }
    auto* sp = dynamic_cast<StreetProperty*>(targets[choice].second);
    std::string before = sp->getBuildingState() == BuildingState::HOTEL ? "Hotel" :
                         std::to_string(static_cast<int>(sp->getBuildingState())) + " rumah";
    sp->demolishOneLevel();
    gui->showMessage("DemolitionCard: satu tingkat bangunan di " + sp->getName() +
                     " milik " + targets[choice].first->getUsername() + " dihancurkan.");
    if (engine->logger != nullptr) {
        engine->logger->log(game->getCurrentTurn(), player->getUsername(),
                            "KARTU", "Pakai DemolitionCard -> hancur 1 bangunan di " +
                            sp->getName() + " (" + sp->getCode() + ") milik " +
                            targets[choice].first->getUsername());
    }
}

CommandResult CommandProcessor::handleUseSkill(Player* player, int index) {
    if (!turn->canUseSkill(player)) {
        gui->showMessage("Kartu kemampuan tidak dapat digunakan sekarang.");
        return CommandResult::INVALID;
    }
    const auto& hand = player->getHandCards();
    if (index < 0 || index >= static_cast<int>(hand.size())) {
        gui->showMessage("Nomor kartu kemampuan tidak valid.");
        return CommandResult::INVALID;
    }
    SkillCard* card = hand[index];

    // Dispatch by card type and apply effect BEFORE removing (some effects need card data)
    if (auto* mc = dynamic_cast<MoveCard*>(card)) {
        player->removeCard(card);
        player->markSkillUsed();
        applyMoveCard(player, mc);
    } else if (auto* tc = dynamic_cast<TeleportCard*>(card)) {
        player->removeCard(card);
        player->markSkillUsed();
        applyTeleportCard(player, tc);
    } else if (auto* dc = dynamic_cast<DiscountCard*>(card)) {
        player->removeCard(card);
        player->markSkillUsed();
        applyDiscountCard(player, dc);
    } else if (auto* sc = dynamic_cast<ShieldCard*>(card)) {
        player->removeCard(card);
        player->markSkillUsed();
        applyShieldCard(player, sc);
    } else if (auto* lc = dynamic_cast<LassoCard*>(card)) {
        player->removeCard(card);
        player->markSkillUsed();
        applyLassoCard(player, lc);
    } else if (auto* demolc = dynamic_cast<DemolitionCard*>(card)) {
        player->removeCard(card);
        player->markSkillUsed();
        applyDemolitionCard(player, demolc);
    } else {
        // JailFreeCard — hanya bisa dipakai saat di penjara (ditangani handleJailedPlayerTurn)
        gui->showMessage("Kartu " + card->getCardName() + " hanya bisa dipakai saat di Penjara.");
        return CommandResult::INVALID;
    }

    if (game->getSkillDeck() != nullptr) game->getSkillDeck()->discard(card);
    return CommandResult::CONTINUE;
}
