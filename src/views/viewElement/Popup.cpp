#include "views/viewElement/Popup.hpp"

Popup::Popup(const View2D &view) : View2D(view) {}

TimedPopup::TimedPopup(const View2D &view, float duration) : Popup(view), popupDuration(duration) {}

IndefinitePopup::IndefinitePopup(const View2D &view) : Popup(view),
                                                       exitButton(Interactable((Vector2){view.getRenderWidth() * 0.1f, view.getRenderWidth() * 0.1f}, true, false, "NULL",
                                                                               [this]()
                                                                               { this->exitButton.setScale(1); }, [this]()
                                                                               { this->closeView = true; }))
{
    exitButton.setRender([this]()
                         {
        this->exitButton.movePosition({this->pos.x + this->getRenderWidth()/2 - this->exitButton.getRenderWidth()/2,
                                       this->pos.y - this->getRenderHeight()/2 + this->exitButton.getRenderHeight()/2});
        DrawRectangle(this->exitButton.getRenderPos().x, this->exitButton.getRenderPos().y,
                      this->exitButton.getRenderWidth(), this->exitButton.getRenderHeight(),
                      this->exitButton.getRenderColor(RED)); });
}

MessagePopup::MessagePopup(const std::string &msg)
    : IndefinitePopup(View2D(getScreenCenter(), {500, 250}, []() {})),
      message(msg),
      okButton(Interactable({200, 50}, true, false, "OK_MESSAGE", []() {}, []() {}))
{
    Vector2 center = getScreenCenter();

    okButton.movePosition(center + Vector2{0, 70});

    okButton.setRender([this]()
                       {
        DrawRectangle(okButton.getRenderPos().x, okButton.getRenderPos().y,
                      okButton.getRenderWidth(), okButton.getRenderHeight(),
                      okButton.getRenderColor(LOGO_RED));

        Vector2 dim = MeasureTextEx(fontMap.at("Orbitron"), "OK", 28, 0);

        DrawTextEx(fontMap.at("Orbitron"), "OK",
                   {okButton.getX() - dim.x / 2, okButton.getY() - 14},
                   28, 0, WHITE); });
}

void MessagePopup::enable()
{
    okButton.enable();
}

void MessagePopup::disable()
{
    okButton.disable();
}

void MessagePopup::interactionCheck()
{
    okButton.interactionCheck();
}

std::string MessagePopup::catchCommand()
{
    if (okButton.catchCommand() == "OK_MESSAGE")
    {
        closeView = true;
        return "OK_MESSAGE";
    }
    return "NULL";
}

void MessagePopup::render()
{
    // background popup
    DrawRectangle(getRenderPos().x, getRenderPos().y,
                  boundingDim.x, boundingDim.y,
                  {40, 40, 40, 230});

    // title
    Vector2 textDim = MeasureTextEx(fontMap.at("Orbitron"), "Message", 28, 0);
    DrawTextEx(fontMap.at("Orbitron"), "Message",
               {pos.x - textDim.x / 2, pos.y - 90},
               28, 0, WHITE);

    // isi pesan
    textDim = MeasureTextEx(fontMap.at("Orbitron"),
                            message.c_str(), 22, 0);
    drawTextLinesWrapped(fontMap.at("Orbitron"), message.c_str(),
               getPos(),22, 0, WHITE, getBoundingDim());

    okButton.render();
}

InputPopup::InputPopup(const std::string &title)
    : IndefinitePopup(View2D(getScreenCenter(), {500, 300}, []() {})),
      title(title),
      inputEntry(Entry({400, 50}, "", 24, "Orbitron", []() {})),
      submitButton(Interactable({200, 50}, true, false, "SUBMIT_INPUT", []() {}, []() {}))
{
    Vector2 center = getScreenCenter();

    inputEntry.movePosition(center + Vector2{0, 0});
    submitButton.movePosition(center + Vector2{0, 80});

    submitButton.setRender([this]()
                           {
        DrawRectangle(submitButton.getRenderPos().x, submitButton.getRenderPos().y,
                      submitButton.getRenderWidth(), submitButton.getRenderHeight(),
                      submitButton.getRenderColor(LOGO_RED));

        Vector2 dim = MeasureTextEx(fontMap.at("Orbitron"), "OK", 28, 0);

        DrawTextEx(fontMap.at("Orbitron"), "OK",
                   {submitButton.getX() - dim.x/2, submitButton.getY() - 14},
                   28, 0, WHITE); });
}

void InputPopup::enable()
{
    inputEntry.enable();
    submitButton.enable();
}

void InputPopup::disable()
{
    inputEntry.disable();
    submitButton.disable();
}

void InputPopup::interactionCheck()
{
    inputEntry.interactionCheck();
    submitButton.interactionCheck();
}

std::string InputPopup::catchCommand()
{
    std::string cmd = submitButton.catchCommand();

    if (cmd == "SUBMIT_INPUT")
    {
        std::string text = inputEntry.getEntryText();
        return text;
    }

    return "NULL";
}

void InputPopup::render()
{
    Vector2 center = {
        (float)GetScreenWidth() / 2,
        (float)GetScreenHeight() / 2};

    DrawRectangle(center.x - 250, center.y - 150, 500, 300, {40, 40, 40, 230});

    Vector2 titleDim = MeasureTextEx(fontMap.at("Orbitron"), title.c_str(), 28, 0);

    DrawTextEx(fontMap.at("Orbitron"), title.c_str(),
               {center.x - titleDim.x / 2, center.y - 100},
               28, 0, WHITE);

    inputEntry.render();
    submitButton.render();
}

ConfirmPopup::ConfirmPopup(const std::string& question)
    : IndefinitePopup(View2D(getScreenCenter(), {600, 220}, []() {})),
      question(question)
{
    yesButton.setGameCommand("YES");
    noButton.setGameCommand("NO");
}

void ConfirmPopup::enable()
{
    float popupWidth = 600;
    float popupHeight = 220;

    float x = (GetScreenWidth() - popupWidth) / 2.0f;
    float y = (GetScreenHeight() - popupHeight) / 2.0f;

    float btnWidth = 140;
    float btnHeight = 50;
    float gap = 40;

    float totalWidth = btnWidth * 2 + gap;
    float startX = x + (popupWidth - totalWidth) / 2.0f;
    float btnY = y + popupHeight - 80;

    yesButton.setPosition({startX, btnY});
    yesButton.setHitboxDim({btnWidth, btnHeight});

    noButton.setPosition({startX + btnWidth + gap, btnY});
    noButton.setHitboxDim({btnWidth, btnHeight});

    yesButton.enable();
    noButton.enable();
}

void ConfirmPopup::disable()
{
    yesButton.disable();
    noButton.disable();
}

void ConfirmPopup::interactionCheck()
{
    yesButton.interactionCheck();
    noButton.interactionCheck();
}

std::string ConfirmPopup::catchCommand()
{
    std::string cmd;

    cmd = yesButton.catchCommand();
    if (cmd != "NULL") return cmd;

    cmd = noButton.catchCommand();
    if (cmd != "NULL") return cmd;

    return "NULL";
}

void ConfirmPopup::render()
{
    float popupWidth = 600;
    float popupHeight = 220;

    float x = (GetScreenWidth() - popupWidth) / 2.0f;
    float y = (GetScreenHeight() - popupHeight) / 2.0f;

    DrawRectangle(0, 0,
                  GetScreenWidth(),
                  GetScreenHeight(),
                  Color{0, 0, 0, 150});

    DrawRectangleRounded(
        {x, y, popupWidth, popupHeight},
        0.1f,
        10,
        Color{40, 40, 40, 240});

    DrawRectangleLinesEx(
        {x, y, popupWidth, popupHeight},
        2,
        GOLD);

    DrawText(
        question.c_str(),
        x + 30,
        y + 40,
        20,
        WHITE);

    float btnWidth = 140;
    float btnHeight = 50;
    float gap = 40;

    float totalWidth = btnWidth * 2 + gap;
    float startX = x + (popupWidth - totalWidth) / 2.0f;
    float btnY = y + popupHeight - 80;

    Rectangle yesRect = {startX, btnY, btnWidth, btnHeight};
    Rectangle noRect = {startX + btnWidth + gap, btnY, btnWidth, btnHeight};

    DrawRectangleRounded(yesRect, 0.3f, 10, GREEN);
    DrawText("YA", yesRect.x + 45, yesRect.y + 15, 20, BLACK);

    DrawRectangleRounded(noRect, 0.3f, 10, RED);
    DrawText("TIDAK", noRect.x + 25, noRect.y + 15, 20, BLACK);

    yesButton.render();
    noButton.render();
}

LoadConfirmPopup::LoadConfirmPopup(const std::string &title, const std::string &placeholder)
    : IndefinitePopup(View2D(getScreenCenter(), {600, 300}, []() {})),
      title(title),
      entry(Entry({400, 50}, placeholder, 24, "Orbitron", []() {})),
      confirmButton(Interactable({200, 50}, true, false, "CONFIRM_LOAD", []() {}, []() {}))
{
    Vector2 center = getScreenCenter();

    entry.movePosition(center + Vector2{0, 20});
    confirmButton.movePosition(center + Vector2{0, 90});

    entry.setOnEnterFunc([this]()
                         {
        if (onSubmit)
            onSubmit(entry.getEntryText());
        closeView = true; });

    confirmButton.setRender([this]()
                            {
        DrawRectangle(confirmButton.getRenderPos().x, confirmButton.getRenderPos().y,
                      confirmButton.getRenderWidth(), confirmButton.getRenderHeight(),
                      confirmButton.getRenderColor(LOGO_RED));

        Vector2 dim = MeasureTextEx(fontMap.at("Orbitron"), "OK", 28, 0);

        DrawTextEx(fontMap.at("Orbitron"), "OK",
                   {confirmButton.getX() - dim.x/2, confirmButton.getY() - 14},
                   28, 0, WHITE); });
}

void LoadConfirmPopup::setOnSubmit(std::function<void(const std::string &)> func)
{
    onSubmit = func;
}

void LoadConfirmPopup::enable()
{
    entry.enable();
    confirmButton.enable();
}

void LoadConfirmPopup::disable()
{
    entry.disable();
    confirmButton.disable();
}

std::string LoadConfirmPopup::catchCommand()
{
    return "NULL";
}

void LoadConfirmPopup::interactionCheck()
{
    entry.interactionCheck();
    confirmButton.interactionCheck();

    if (confirmButton.catchCommand() == "CONFIRM_LOAD")
    {
        if (onSubmit)
            onSubmit(entry.getEntryText());

        closeView = true;
    }
}

void LoadConfirmPopup::render()
{
    DrawRectangle(getRenderPos().x, getRenderPos().y,
                  boundingDim.x, boundingDim.y,
                  {50, 50, 50, 230});

    DrawTextEx(fontMap.at("Orbitron"), title.c_str(),
               {pos.x - 200, pos.y - 80}, 28, 0, WHITE);

    entry.render();
    confirmButton.render();
}

ExceptionPopup::ExceptionPopup(int errorCode, const std::string &errorMessage) : IndefinitePopup(View2D(getScreenCenter(), {480, 360}, []() {})),
                                                                                 errorCode(errorCode),
                                                                                 errorMessage(errorMessage),
                                                                                 okButton(Interactable({320, 50}, true, false, "NULL", []() {}, [this]()
                                                                                                       { this->closeView = true; }))
{
    okButton.movePosition(this->pos.x, this->pos.y + this->getRenderDim().y / 2 - this->okButton.getBoundingHeight() / 2 - 20);

    okButton.setRender([this]()
                       {
        DrawRectangle(this->okButton.getRenderPos().x, this->okButton.getRenderPos().y,
                      this->okButton.getRenderWidth(), this->okButton.getRenderHeight(),
                      this->okButton.getRenderColor(RED));
                      
        Vector2 textMeasure = MeasureTextEx(fontMap["Orbitron"], "OK", this->okButton.getRenderFontSize(36), 1);
        DrawTextEx(fontMap["Orbitron"], "OK", this->okButton.getPos() - textMeasure/2, 
                   this->okButton.getRenderFontSize(36), 1, this->okButton.getRenderColor(WHITE)); });
}

void ExceptionPopup::enable()
{
    okButton.enable();
}

void ExceptionPopup::disable()
{
    okButton.disable();
}

string ExceptionPopup::catchCommand()
{
    return okButton.catchCommand();
}

void ExceptionPopup::interactionCheck()
{
    okButton.interactionCheck();
}

void ExceptionPopup::render()
{
    animationCheck();

    DrawRectangle(this->getRenderPos().x, this->getRenderPos().y,
                  this->getRenderWidth(), this->getRenderHeight(),
                  this->getRenderColor(RAYWHITE));

    DrawRectangle(this->getRenderPos().x, this->getRenderPos().y,
                  this->getRenderWidth(), this->getRenderWidth() * 0.1,
                  this->getRenderColor(MAROON));

    std::string fullText = "Code: " + std::to_string(errorCode) + "\n \n" + errorMessage;
    drawTextLinesWrapped(fontMap["Orbitron"], fullText, pos,
                         getRenderFontSize(28), 1, getRenderColor(BLACK), getRenderDim() - (Vector2){20, 0});

    okButton.render();
}

PropertyPopup::PropertyPopup(
    const std::string &name,
    const std::string &type,
    const std::string &status,
    int buyPrice,
    int mortgageValue,
    int levelOrCount,
    bool isOtherPlayer,
    const std::string &ownerName,
    const std::string &colorGroup,
    const std::vector<int> &rentTable,
    int buildCost,
    const std::vector<int> &railroadRent,
    const std::vector<int> &utilityMultiplier)
    : IndefinitePopup(View2D(getScreenCenter(), {520, 420}, []() {})),
      name(name),
      type(type),
      status(status),
      ownerName(ownerName),
      colorGroup(colorGroup),
      rentTable(rentTable),
      buildCost(buildCost),
      buyPrice(buyPrice),
      mortgageValue(mortgageValue),
      railroadRent(railroadRent),
      utilityMultiplier(utilityMultiplier),
      levelOrCount(levelOrCount),
      isOtherPlayer(isOtherPlayer),
      actionCommand("")
{
    // posisi tombol
    Vector2 btnPos = {
        this->pos.x,
        this->pos.y + this->getRenderDim().y / 2 - 60};

    // BUTTON LOGIC
    // BANK
    if (status == "BANK")
    {
        if (type == "STREET")
        {
            addButton("BUY", "BUY_PROPERTY");
            addButton("SKIP", "SKIP_PROPERTY");
        }
        else
        {
            addButton("OK", "ACK");
        }
    }

    // OWNED
    else if (status == "OWNED")
    {

        if (isOtherPlayer)
        {
            addButton("PAY", "PAY_RENT");
        }
        else
        {
            // milik sendiri

            if (type == "STREET")
            {
                if (levelOrCount == 0)
                {
                    addButton("BUILD", "BUILD_PROPERTY");
                    addButton("MORTGAGE", "MORTGAGE_PROPERTY");
                }
                else if (levelOrCount < 5)
                {
                    addButton("BUILD", "BUILD_PROPERTY");
                }
            }
            else
            {
                addButton("MORTGAGE", "MORTGAGE_PROPERTY");
            }

            addButton("OK", "ACK");
        }
    }

    // MORTGAGED
    else if (status == "MORTGAGED")
    {
        addButton("REDEEM", "REDEEM_PROPERTY");
        addButton("OK", "ACK");
    }

    float startY = this->pos.y + this->getRenderDim().y / 2 - 60;

    for (int i = 0; i < actionButtons.size(); i++)
    {
        actionButtons[i].movePosition({this->pos.x,
                                       startY - i * 70});

        actionButtons[i].setRender([this, i]()
                                   {
            auto& btn = actionButtons[i];

            DrawRectangle(
                btn.getRenderPos().x,
                btn.getRenderPos().y,
                btn.getRenderWidth(),
                btn.getRenderHeight(),
                btn.getRenderColor(DARKGREEN)
            );

            std::string label = btn.getGameCommand();

            Vector2 t = MeasureTextEx(fontMap["Orbitron"],
                                    label.c_str(), 28, 1);

            DrawTextEx(fontMap["Orbitron"],
                    label.c_str(),
                    btn.getPos() - t/2,
                    28, 1,
                    WHITE); });
    }
    std::cout << "PropertyPopup constructed\n";
}

void PropertyPopup::enable()
{
    for (auto &btn : actionButtons)
        btn.enable();
    exitButton.enable();
}

void PropertyPopup::disable()
{
    for (auto &btn : actionButtons)
        btn.disable();
    exitButton.disable();
}

void PropertyPopup::addButton(const std::string &label, const std::string &command)
{
    Interactable btn(
        {220, 50},
        true,
        false,
        label,
        []() {},
        [this, command]()
        {
            this->actionCommand = command;
            this->closeView = true;
        });

    actionButtons.push_back(btn);
}

void PropertyPopup::interactionCheck()
{
    for (auto &btn : actionButtons)
        btn.interactionCheck();
    exitButton.interactionCheck();
}

string PropertyPopup::catchCommand()
{
    if (actionCommand != "")
        return actionCommand;

    for (auto &btn : actionButtons)
    {
        string cmd = btn.catchCommand();
        if (cmd != "NULL")
            return cmd;
    }

    return "NULL";
}

// Helper
std::string PropertyPopup::buildDetails() const
{
    std::string s;

    // STATUS
    s += "Status: " + status;
    if (status == "OWNED" && ownerName != "")
        s += " (" + ownerName + ")";
    s += "\n\n";
    // std::cout << "Building details status\n";

    // TYPE
    s += "Type: " + type + "\n";
    // std::cout << "Building details type\n";

    // STREET
    if (type == "STREET")
    {
        s += "Color: " + colorGroup + "\n";

        if (status == "BANK")
        {
            s += "Buy Price: " + std::to_string(buyPrice) + "\n";
            return s;
        }

        if (status == "OWNED")
        {
            if (!rentTable.empty())
            {
                int idx = std::max(0, levelOrCount);
                idx = std::min(idx, (int)rentTable.size() - 1);
                int rent = rentTable[idx];
                s += "Current Rent: " + std::to_string(rent) + "\n";
            }

            if (!isOtherPlayer)
            {
                s += "Build Cost: " + std::to_string(buildCost) + "\n";
                s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
                s += "Level: " + std::to_string(levelOrCount) + "\n";
            }
        }

        if (status == "MORTGAGED")
        {
            s += "This property is mortgaged.\n";
            if (!isOtherPlayer)
            {
                s += "Mortgage Value: " + std::to_string(mortgageValue) + "\n";
            }
        }
        // std::cout << "Building details street\n";
    }

    // RAILROAD
    else if (type == "RAILROAD")
    {
        if (status == "OWNED")
        {
            if (!railroadRent.empty())
            {
                int idx = std::max(0, levelOrCount - 1);
                idx = std::min(idx, (int)railroadRent.size() - 1);
                int rent = railroadRent[idx];
                s += "Rent: " + std::to_string(rent) + "\n";
            }

            s += "Owned Count: " + std::to_string(levelOrCount) + "\n";

            if (!isOtherPlayer)
            {
                s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
            }
        }

        else if (status == "MORTGAGED")
        {
            s += "This railroad is mortgaged.\n";

            if (!isOtherPlayer)
            {
                s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
            }
        }
        // std::cout << "Building details railroad\n";
    }

    // UTILITY
    else if (type == "UTILITY")
    {
        if (status == "OWNED")
        {
            if (!utilityMultiplier.empty())
            {
                int idx = std::max(0, levelOrCount - 1);
                idx = std::min(idx, (int)utilityMultiplier.size() - 1);
                int mult = utilityMultiplier[idx];
                s += "Multiplier: x" + std::to_string(mult) + "\n";
            }

            s += "Owned Count: " + std::to_string(levelOrCount) + "\n";

            if (!isOtherPlayer)
            {
                s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
            }
        }

        else if (status == "MORTGAGED")
        {
            s += "This utility is mortgaged.\n";

            if (!isOtherPlayer)
            {
                s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
            }
        }
        // std::cout << "Building details utility\n";
    }

    return s;
}

void PropertyPopup::render()
{
    animationCheck();

    // background
    DrawRectangle(getRenderPos().x, getRenderPos().y,
                  getRenderWidth(), getRenderHeight(),
                  RAYWHITE);
    // std::cout << "Setting up background\n";

    // header
    DrawRectangle(getRenderPos().x, getRenderPos().y,
                  getRenderWidth(), getRenderHeight() * 0.125,
                  RED);
    // std::cout << "Setting up header\n";

    // title
    Vector2 t = MeasureTextEx(fontMap["Orbitron"], name.c_str(), 28, 1);
    DrawTextEx(fontMap["Orbitron"], name.c_str(),
               pos - Vector2{t.x / 2, getRenderHeight() / 2 - 10},
               28, 1, WHITE);
    // std::cout << "Setting up title\n";

    // details
    std::string text = buildDetails();

    // menghapus baris kosong
    std::stringstream ss(text);
    std::string line;
    std::string fixedText;

    while (std::getline(ss, line))
    {
        if (!line.empty())
        {
            fixedText += line + "\n";
        }
    }
    // std::cout << "Built details done\n";

    drawTextLinesWrapped(
        fontMap["Orbitron"],
        fixedText,
        pos,
        22,
        1,
        BLACK,
        getRenderDim() - (Vector2){40, 0});
    // std::cout << "Drawing text lines\n";

    for (auto &btn : actionButtons)
        btn.render();
    exitButton.render();
    // std::cout << "Rendering popup\n";
}

// Contoh pakai Popup untuk Street di view testing (Belum implementasi color group bonus)
// app.loadPopup(new PropertyPopup(
//     "Medan",                         // nama properti
//     "STREET",                        // tipe properti ("STREET", "RAILROAD", atau "UTILITY")
//     "OWNED",                         // status kepemilikan ("BANK", "OWNED", atau "MORTGAGED")
//     200,                             // harga beli
//     100,                             // nilai gadai (mortgage value)

//     2,                               // 2 rumah dibangun
//     true,                            // milik orang lain

//     "Player2",                       // owner properti
//     "YELLOW",                        // warna group (hanya untuk Street)
//     {20, 40, 60, 100, 150, 200},     // rentTable untuk Street (index 0-5: 0 rumah - hotel)
//     50                               // harga bangun (hanya untuk Street)
// ));                                  // railroadRent dan utilityMultiplier DIKOSONGKAN karena bukan Railroad/Utility

// Contoh pakai Popup untuk Railroad di view testing
// app.loadPopup(new PropertyPopup(
//     "Stasiun Gambir",                // nama properti
//     "RAILROAD",                      // tipe properti ("STREET", "RAILROAD", atau "UTILITY")
//     "OWNED",                         // status kepemilikan ("BANK", "OWNED", atau "MORTGAGED")
//     0,                               // tidak ada harga beli untuk Railroad (dianggap 0 karena tidak relevan)
//     100,                             // nilai gadai (mortgage value)

//     3,                               // punya 3 railroad
//     true,                            // milik orang lain

//     "Player1",                       // owner properti
//     "",                              // warna group DIKOSONGKAN karena bukan Street
//     {},                              // rentTable DIKOSONGKAN karena bukan Street
//     0,                               // harga bangun DIKOSONGKAN karena bukan Street
//     {25, 50, 100, 200}               // railroadRent berdasarkan jumlah kepemilikan Railroad yang sama (index 0-3: punya 1-4 railroad)
// ));                                  // buildCost dan utilityMultiplier DIKOSONGKAN karena bukan Street/Utility

// Contoh pakai Popup untuk Utility di view testing
// app.loadPopup(new PropertyPopup(
//     "PLN",                           // nama properti
//     "UTILITY",                       // tipe properti ("STREET", "RAILROAD", atau "UTILITY")
//     "OWNED",                         // status kepemilikan ("BANK", "OWNED", atau "MORTGAGED")
//     0,                               // tidak ada harga beli untuk Utility (dianggap 0 karena tidak relevan)
//     75,                              // nilai gadai (mortgage value)

//     2,                               // punya 2 utility
//     true,                            // milik orang lain

//     "Player2",                       // owner properti
//     "",                              // warna group DIKOSONGKAN karena bukan Street
//     {},                              // rentTable DIKOSONGKAN karena bukan Street
//     0,                               // harga bangun DIKOSONGKAN karena bukan Street
//     {},                              // railroadRent DIKOSONGKAN karena bukan Railroad
//     {4, 10}                          // utilityMultiplier berdasarkan jumlah kepemilikan Utility yang sama (index 0-1: punya 1-2 utility)
// ));