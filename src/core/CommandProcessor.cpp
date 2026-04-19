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

CommandResult CommandProcessor::process(const std::string& command, Player* player) {
    if (command.empty() || command == "NULL") return CommandResult::CONTINUE;

    auto tokens = tokenize(command);
    if (tokens.empty()) return CommandResult::CONTINUE;

    std::string cmd = normalize(tokens[0]);

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
        if (cmd == "SIMPAN") {
            if (tokens.size() < 2) { gui->showMessage("Format: SIMPAN <file>"); return CommandResult::INVALID; }
            return handleSave(tokens[1]);
        }
        if (cmd == "AKHIRI_GILIRAN") return handleEndTurn(player);

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

    if (manual) dice->setManual(d1, d2);
    else        dice->rollRandom();

    int total = dice->getTotal();
    gui->renderDice(dice->getDie1(), dice->getDie2());
    game->setLastDiceTotal(total);
    player->markRolled();

    if (dice->isDouble()) player->incrementConsecutiveDoubles();
    else                  player->resetConsecutiveDoubles();

    Tile* landed = turn->processMovement(player, total);
    turn->setPhase(TurnPhase::POST_ROLL);
    turn->markActed();

    if (landed) engine->handleTileLanding(player, landed);

    if (player->getStatus() == PlayerStatus::BANKRUPT) {
        if (engine->checkWinCondition()) return CommandResult::GAME_OVER;
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
    gui->showMessage("Simpan belum diimplementasi: " + file);
    return CommandResult::SAVED_MID_TURN;
}

CommandResult CommandProcessor::handleEndTurn(Player* player) {
    if (!player->hasRolled()) {
        gui->showMessage("Lempar dadu dulu sebelum mengakhiri giliran.");
        return CommandResult::INVALID;
    }
    turn->endTurn(player);
    return CommandResult::END_TURN;
}
