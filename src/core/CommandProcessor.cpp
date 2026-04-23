#include "core/CommandProcessor.hpp"

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
           cmd == "CETAK_DEED" ||
           cmd == "CETAK_PROPERTI" ||
           cmd == "CETAK_LOG" ||
           cmd == "SIMPAN" ||
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
        if (cmd == "CETAK_DEED") {
            if (tokens.size() < 2) { gui->showMessage("Format: CETAK_DEED <kode>"); return CommandResult::INVALID; }
            return handlePrintDeed(tokens[1]);
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
            if (tokens.size() < 2) { gui->showMessage("Format: GUNAKAN_KEMAMPUAN <idx>"); return CommandResult::INVALID; }
            return handleUseSkill(player, std::stoi(tokens[1]));
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
            return handleSave(tokens[1]);
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
        return CommandResult::INVALID;
    } catch (const std::exception& e) {
        gui->showMessage(std::string("Error: ") + e.what());
        return CommandResult::INVALID;
    }
}

CommandResult CommandProcessor::handleRoll(Player* player, bool manual, int d1, int d2) {
    if (!turn->canRoll(player)) {
        gui->showMessage("Tidak dapat melempar dadu sekarang.");
        return CommandResult::INVALID;
    }

    if (manual) {
        gui->showMessage("Dadu diatur secara manual.");
        dice->setManual(d1, d2);
    } else {
        gui->showMessage("Mengocok dadu...");
        dice->rollRandom();
    }

    int total = dice->getTotal();
    bool rolledDouble = dice->isDouble();
    gui->renderDice(dice->getDie1(), dice->getDie2());
    game->setLastDiceTotal(total);
    player->markRolled();

    if (rolledDouble) {
        player->incrementConsecutiveDoubles();
        if (player->getConsecutiveDoubles() >= 3) {
            Board* board = game->getBoard();
            if (board != nullptr && board->getJailTile() != nullptr) {
                player->setPosition(board->getJailTile()->getIndex());
            }
            player->setStatus(PlayerStatus::JAILED);
            gui->showMessage("Dadu double ketiga berturut-turut.");
            gui->showMessage("Bidak tidak digerakkan sesuai hasil dadu dan kamu langsung masuk Penjara.");
            turn->endTurn(player);
            return CommandResult::END_TURN;
        }
    } else {
        player->resetConsecutiveDoubles();
    }

    Tile* landed = turn->processMovement(player, total);
    turn->setPhase(TurnPhase::POST_ROLL);
    turn->markActed();

    std::string landedName = landed ? landed->getName() : "?";
    gui->renderMovement(player->getUsername(), total, landedName);

    if (landed) engine->handleTileLanding(player, landed);

    if (player->getStatus() == PlayerStatus::JAILED) {
        gui->showMessage("Giliran berakhir karena kamu berada di Penjara.");
        turn->endTurn(player);
        return CommandResult::END_TURN;
    }

    if (player->getStatus() == PlayerStatus::BANKRUPT) {
        if (engine->checkWinCondition()) return CommandResult::GAME_OVER;
        return CommandResult::CONTINUE;
    }

    if (player->hasPendingFestival()) {
        if (rolledDouble) {
            gui->showMessage("Lanjutkan dengan FESTIVAL <kode_properti> untuk menyelesaikan aksi petak ini.");
            gui->showMessage("Setelah itu, gunakan LEMPAR_DADU atau ATUR_DADU <x> <y> untuk lemparan bonus.");
        } else {
            gui->showMessage("Lanjutkan dengan FESTIVAL <kode_properti>, lalu gunakan command lain atau AKHIRI_GILIRAN.");
        }
    } else if (rolledDouble) {
        gui->showMessage("Dadu double. Kamu mendapat lemparan bonus.");
        gui->showMessage("Gunakan LEMPAR_DADU atau ATUR_DADU <x> <y> untuk lemparan berikutnya.");
    } else {
        gui->showMessage("Giliran lempar dadu selesai. Gunakan command lain atau AKHIRI_GILIRAN.");
        gui->showMessage("Ketik HELP untuk melihat command yang tersedia.");
    }

    return CommandResult::CONTINUE;
}

CommandResult CommandProcessor::handlePrintBoard() {
    gui->renderBoard(*game);
    return CommandResult::CONTINUE;
}

CommandResult CommandProcessor::handlePrintDeed(const std::string& code) {
    Tile* tile = game->getBoard()->getTile(code);
    auto* pt = dynamic_cast<PropertyTile*>(tile);
    if (!pt || !pt->getProperty()) {
        gui->showMessage("Kode properti tidak ditemukan: " + code);
        return CommandResult::INVALID;
    }
    gui->renderProperty(*pt->getProperty());
    return CommandResult::CONTINUE;
}

CommandResult CommandProcessor::handlePrintProperty(Player* player) {
    const auto& owned = player->getOwnedProperties();
    if (owned.empty()) {
        gui->showMessage(player->getUsername() + " belum memiliki properti.");
        return CommandResult::CONTINUE;
    }
    for (Property* p : owned) gui->renderProperty(*p);
    return CommandResult::CONTINUE;
}

CommandResult CommandProcessor::handleMortgage(Player* player, const std::string& code) {
    Tile* tile = game->getBoard()->getTile(code);
    auto* pt = dynamic_cast<PropertyTile*>(tile);
    Property* prop = pt ? pt->getProperty() : nullptr;
    if (!prop || prop->getOwner() != player) {
        gui->showMessage("Anda tidak memiliki properti " + code);
        return CommandResult::INVALID;
    }
    if (prop->isMortgaged()) {
        gui->showMessage("Properti sudah digadai.");
        return CommandResult::INVALID;
    }
    if (auto* sp = dynamic_cast<StreetProperty*>(prop)) {
        if (sp->getBuildingState() != BuildingState::NONE) {
            gui->showMessage("Jual bangunan dulu sebelum menggadai.");
            return CommandResult::INVALID;
        }
    }
    int value = prop->getMortgageValue();
    prop->setStatus(PropertyStatus::MORTGAGED);
    player->addMoney(value);
    gui->showMessage("Properti " + code + " digadai, menerima " + std::to_string(value));
    return CommandResult::CONTINUE;
}

CommandResult CommandProcessor::handleRedeem(Player* player, const std::string& code) {
    Tile* tile = game->getBoard()->getTile(code);
    auto* pt = dynamic_cast<PropertyTile*>(tile);
    Property* prop = pt ? pt->getProperty() : nullptr;
    if (!prop || prop->getOwner() != player) {
        gui->showMessage("Anda tidak memiliki properti " + code);
        return CommandResult::INVALID;
    }
    if (!prop->isMortgaged()) {
        gui->showMessage("Properti tidak sedang digadai.");
        return CommandResult::INVALID;
    }
    int cost = static_cast<int>(prop->getMortgageValue() * 1.1);
    if (!player->canAfford(cost)) {
        gui->showMessage("Saldo tidak cukup untuk menebus (" + std::to_string(cost) + ")");
        return CommandResult::INVALID;
    }
    player->deductMoney(cost);
    prop->setStatus(PropertyStatus::OWNED);
    gui->showMessage("Properti " + code + " ditebus seharga " + std::to_string(cost));
    return CommandResult::CONTINUE;
}

CommandResult CommandProcessor::handleBuild(Player* player, const std::string& code) {
    Tile* tile = game->getBoard()->getTile(code);
    auto* pt = dynamic_cast<PropertyTile*>(tile);
    auto* sp = pt ? dynamic_cast<StreetProperty*>(pt->getProperty()) : nullptr;
    if (!sp || sp->getOwner() != player) {
        gui->showMessage("Anda tidak memiliki street property " + code);
        return CommandResult::INVALID;
    }
    if (sp->isMortgaged()) {
        gui->showMessage("Tidak dapat membangun di properti yang digadai.");
        return CommandResult::INVALID;
    }
    if (!player->ownsFullColorGroup(sp->getColorGroup())) {
        gui->showMessage("Butuh satu set warna lengkap untuk membangun.");
        return CommandResult::INVALID;
    }
    if (sp->canBuildHouse()) {
        if (sp->buildHouse()) {
            gui->showMessage("Rumah dibangun di " + code);
            return CommandResult::CONTINUE;
        }
    } else if (sp->canBuildHotel()) {
        if (sp->buildHotel()) {
            gui->showMessage("Hotel dibangun di " + code);
            return CommandResult::CONTINUE;
        }
    }
    gui->showMessage("Tidak dapat membangun lagi di " + code);
    return CommandResult::INVALID;
}

CommandResult CommandProcessor::handleUseSkill(Player* player, int index) {
    if (!turn->canUseSkill(player)) {
        gui->showMessage("Tidak dapat menggunakan kartu kemampuan sekarang.");
        return CommandResult::INVALID;
    }
    const auto& hand = player->getHandCards();
    if (index < 0 || index >= static_cast<int>(hand.size())) {
        gui->showMessage("Indeks kartu tidak valid.");
        return CommandResult::INVALID;
    }
    SkillCard* card = hand[index];
    player->removeCard(card);
    player->markSkillUsed();
    gui->showMessage("Kartu kemampuan digunakan: " + card->getCardName());
    return CommandResult::CONTINUE;
}

CommandResult CommandProcessor::handlePrintLog(int nLast) {
    auto* logger = engine->logger;
    std::vector<LogEntry> entries = (nLast > 0)
        ? logger->getRecentLog(nLast)
        : logger->getFullLog();
    gui->renderLog(entries);
    return CommandResult::CONTINUE;
}

CommandResult CommandProcessor::handleSave(const std::string& file) {
    if (engine->saveLoadManager == nullptr) {
        gui->showMessage("SaveLoadManager belum tersedia.");
        return CommandResult::INVALID;
    }
    if (engine->saveLoadManager->save(file)) {
        gui->showMessage("Game disimpan ke " + file);
        return CommandResult::SAVED_MID_TURN;
    }
    gui->showMessage("Gagal menyimpan ke " + file);
    return CommandResult::INVALID;
}

CommandResult CommandProcessor::handleFestival(Player* player, const std::string& code) {
    if (!player->hasPendingFestival()) {
        gui->showMessage("Tidak ada festival yang menunggu konfirmasi.");
        return CommandResult::INVALID;
    }
    Property* target = nullptr;
    for (Property* p : player->getOwnedProperties()) {
        if (p->getCode() == code) { target = p; break; }
    }
    if (target == nullptr) {
        gui->showMessage("Properti " + code + " tidak ditemukan atau bukan milik Anda.");
        return CommandResult::INVALID;
    }
    if (target->isMortgaged()) {
        gui->showMessage("Properti " + code + " sedang digadai, tidak dapat difestivalkan.");
        return CommandResult::INVALID;
    }
    target->activateFestival();
    player->setPendingFestival(false);
    gui->showMessage("Festival aktif di " + target->getName() + "! Sewa berlipat selama "
                     + std::to_string(target->getFestivalDuration()) + " giliran.");
    if (isAwaitingBonusRoll(player)) {
        gui->showMessage("Karena lemparan sebelumnya double, lanjutkan dengan LEMPAR_DADU atau ATUR_DADU <x> <y>.");
    } else {
        gui->showMessage("Gunakan command lain atau AKHIRI_GILIRAN.");
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
        gui->showMessage("Pilih properti festival dulu: FESTIVAL <kode>");
        return CommandResult::INVALID;
    }
    turn->endTurn(player);
    return CommandResult::END_TURN;
}

CommandResult CommandProcessor::handleHelp(Player* player) {
    gui->showMessage("Daftar command umum:");
    gui->showMessage("CETAK_PAPAN | CETAK_DEED <kode> | CETAK_PROPERTI | CETAK_LOG [n]");

    if (player == nullptr) {
        gui->showMessage("LEMPAR_DADU | ATUR_DADU <x> <y> | AKHIRI_GILIRAN");
        return CommandResult::CONTINUE;
    }

    if (!player->hasRolled()) {
        gui->showMessage("Sebelum lempar dadu: LEMPAR_DADU | ATUR_DADU <x> <y> | GUNAKAN_KEMAMPUAN <idx> | SIMPAN <file>");
    } else if (player->hasPendingFestival()) {
        gui->showMessage("Saat aksi Festival: FESTIVAL <kode_properti> | CETAK_PAPAN | CETAK_PROPERTI | CETAK_LOG [n]");
        if (player->getConsecutiveDoubles() > 0) {
            gui->showMessage("Setelah Festival selesai, lanjutkan dengan LEMPAR_DADU atau ATUR_DADU <x> <y>.");
        }
    } else if (isAwaitingBonusRoll(player)) {
        gui->showMessage("Saat menunggu lemparan bonus: LEMPAR_DADU | ATUR_DADU <x> <y> | CETAK_PAPAN | CETAK_DEED <kode> | CETAK_PROPERTI | CETAK_LOG [n] | SIMPAN <file>");
    } else {
        gui->showMessage("Setelah lempar dadu: GADAI <kode> | TEBUS <kode> | BANGUN <kode> | CETAK_PAPAN | CETAK_PROPERTI | AKHIRI_GILIRAN");
    }

    gui->showMessage("Khusus keluar game: EXIT");
    return CommandResult::CONTINUE;
}
