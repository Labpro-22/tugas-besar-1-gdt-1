#include "views/viewElement/Popup.hpp"


Popup::Popup(const View2D& view) : View2D(view) {}

TimedPopup::TimedPopup(const View2D& view, float duration) : Popup(view), popupDuration(duration) {}

IndefinitePopup::IndefinitePopup(const View2D& view) : 
    Popup(view), 
    exitButton(Interactable((Vector2){view.getRenderWidth()*0.1f, view.getRenderWidth()*0.1f}, true, false, "NULL",
                [this](){this->exitButton.setScale(1);}, [this](){ this->closeView = true;})) 
    {
    exitButton.setRender([this]() {
        this->exitButton.movePosition({this->pos.x + this->getRenderWidth()/2 - this->exitButton.getRenderWidth()/2,
                                       this->pos.y - this->getRenderHeight()/2 + this->exitButton.getRenderHeight()/2});
        DrawRectangle(this->exitButton.getRenderPos().x, this->exitButton.getRenderPos().y,
                      this->exitButton.getRenderWidth(), this->exitButton.getRenderHeight(),
                      this->exitButton.getRenderColor(RED));
    });
}

LoadConfirmPopup::LoadConfirmPopup(const string filePath) : 
    IndefinitePopup(View2D(getScreenCenter(), {480, 360}, [](){})),
    filePath(filePath),
    confirmButton(Interactable({320, 50}, true, false, "LOAD " + filePath, [](){}, [this](){ this->closeView = true;})) 
    {
    confirmButton.movePosition(this->pos.x, this->pos.y + this->getRenderDim().y/2 - this->confirmButton.getBoundingHeight()/2 - 20);
    confirmButton.setRender([this](){
        DrawRectangle(this->confirmButton.getRenderPos().x, this->confirmButton.getRenderPos().y,
                      this->confirmButton.getRenderWidth(), this->confirmButton.getRenderHeight(),
                      this->confirmButton.getRenderColor(LIME));
        Vector2 textMeasure = MeasureTextEx(fontMap["Orbitron"], "Confirm", this->confirmButton.getRenderFontSize(36), 1);
        DrawTextEx(fontMap["Orbitron"], "Confirm", this->confirmButton.getPos() - textMeasure/2, 
                   this->confirmButton.getRenderFontSize(36), 1, this->confirmButton.getRenderColor(WHITE));
    });
}

void LoadConfirmPopup::enable() {
    confirmButton.enable();
    exitButton.enable();
}

void LoadConfirmPopup::disable() {
    confirmButton.disable();
    exitButton.disable();
}

const string LoadConfirmPopup::catchCommand() {
    return confirmButton.catchCommand();
}

void LoadConfirmPopup::interactionCheck() {
    confirmButton.interactionCheck();
    exitButton.interactionCheck();
}

void LoadConfirmPopup::render() {
    animationCheck();
    DrawRectangle(this->getRenderPos().x, this->getRenderPos().y,
                  this->getRenderWidth(), this->getRenderHeight(),
                  this->getRenderColor(BEIGE));
    DrawRectangle(this->getRenderPos().x, this->getRenderPos().y,
                  this->getRenderWidth(), this->getRenderWidth()*0.1,
                  this->getRenderColor(BROWN));
    drawTextLinesWrapped(fontMap["Orbitron"], "Load save data from the following path?\n \n" + filePath, pos, 
                         getRenderFontSize(36), 1, getRenderColor(BLACK), getRenderDim() - (Vector2){20, 0});
    exitButton.render();
    confirmButton.render();
}


ExceptionPopup::ExceptionPopup(int errorCode, const std::string &errorMessage) : 
    IndefinitePopup(View2D(getScreenCenter(), {480, 360}, [](){})),
    errorCode(errorCode),
    errorMessage(errorMessage),
    okButton(Interactable({320, 50}, true, false, "NULL", [](){}, [this](){ this->closeView = true;})) 
    {
    okButton.movePosition(this->pos.x, this->pos.y + this->getRenderDim().y/2 - this->okButton.getBoundingHeight()/2 - 20);
    
    okButton.setRender([this](){
        DrawRectangle(this->okButton.getRenderPos().x, this->okButton.getRenderPos().y,
                      this->okButton.getRenderWidth(), this->okButton.getRenderHeight(),
                      this->okButton.getRenderColor(RED));
                      
        Vector2 textMeasure = MeasureTextEx(fontMap["Orbitron"], "OK", this->okButton.getRenderFontSize(36), 1);
        DrawTextEx(fontMap["Orbitron"], "OK", this->okButton.getPos() - textMeasure/2, 
                   this->okButton.getRenderFontSize(36), 1, this->okButton.getRenderColor(WHITE));
    });
}

void ExceptionPopup::enable() {
    okButton.enable();
}

void ExceptionPopup::disable() {
    okButton.disable();
}

const string ExceptionPopup::catchCommand() {
    return okButton.catchCommand();
}

void ExceptionPopup::interactionCheck() {
    okButton.interactionCheck();
}

void ExceptionPopup::render() {
    animationCheck();
    
    DrawRectangle(this->getRenderPos().x, this->getRenderPos().y,
                  this->getRenderWidth(), this->getRenderHeight(),
                  this->getRenderColor(RAYWHITE));
                  
    DrawRectangle(this->getRenderPos().x, this->getRenderPos().y,
                  this->getRenderWidth(), this->getRenderWidth()*0.1,
                  this->getRenderColor(MAROON));
                  
    std::string fullText = "Code: " + std::to_string(errorCode) + "\n \n" + "Message: " + errorMessage;
    drawTextLinesWrapped(fontMap["Orbitron"], fullText, pos, 
                         getRenderFontSize(28), 1, getRenderColor(BLACK), getRenderDim() - (Vector2){20, 0});
                         
    okButton.render();
}

PropertyPopup::PropertyPopup(
    const std::string& name,
    int buyPrice,
    int mortgageValue,
    bool isOtherPlayer,
    const std::string& ownerName
)
: IndefinitePopup(View2D(getScreenCenter(), {520, 420}, [](){})),
  name(name),
  ownerName(ownerName),
  isOtherPlayer(isOtherPlayer),
  buyPrice(buyPrice),
  mortgageValue(mortgageValue),
  actionCommand("")
{
}

void PropertyPopup::addButton(const std::string& label, const std::string& command) {
    actionButtons.emplace_back(
        Vector2{200, 50},
        true,
        false,
        label,
        [](){},
        [this, command]() {
            this->actionCommand = command;
            this->closeView = true;
        }
    );

    int idx = actionButtons.size() - 1;

    actionButtons[idx].setRender([this, idx]() {
        auto& btn = actionButtons[idx];

        DrawRectangle(
            btn.getRenderPos().x,
            btn.getRenderPos().y,
            btn.getRenderWidth(),
            btn.getRenderHeight(),
            btn.getRenderColor(DARKGREEN)
        );

        std::string label = btn.getGameCommand();

        Vector2 t = MeasureTextEx(fontMap["Orbitron"], label.c_str(), 24, 1);

        DrawTextEx(
            fontMap["Orbitron"],
            label.c_str(),
            btn.getPos() - t/2,
            24,
            1,
            WHITE
        );
    });
}

void PropertyPopup::enable() {
    for (auto& b : actionButtons) b.enable();
    exitButton.enable();
}

void PropertyPopup::disable() {
    for (auto& b : actionButtons) b.disable();
    exitButton.disable();
}

void PropertyPopup::interactionCheck() {
    for (auto& b : actionButtons) b.interactionCheck();
    exitButton.interactionCheck();
}

const std::string PropertyPopup::catchCommand() {
    if (actionCommand.empty()) return "NULL";

    std::string temp = actionCommand;
    actionCommand = "";
    return temp;
}

void PropertyPopup::render() {
    animationCheck();

    float x = getRenderPos().x;
    float y = getRenderPos().y;
    float w = getRenderWidth();
    float h = getRenderHeight();

    // ===== BACKGROUND =====
    DrawRectangle(x, y, w, h, RAYWHITE);

    // ===== HEADER =====
    float headerH = h * 0.2f;
    DrawRectangle(x, y, w, headerH, RED);

    // ===== TITLE =====
    Vector2 t = MeasureTextEx(fontMap["Orbitron"], name.c_str(), 26, 1);
    DrawTextEx(
        fontMap["Orbitron"],
        name.c_str(),
        {x + w/2 - t.x/2, y + headerH/2 - t.y/2},
        26,
        1,
        WHITE
    );

    // ===== DETAILS (DI BAWAH HEADER) =====
    float currentY = y + headerH + 80;

    std::string text = buildDetails();

    drawTextLinesWrapped(
        fontMap["Orbitron"],
        text,
        {x + w/2, currentY},
        20,
        1,
        BLACK,
        {w - 40, h}
    );

    // ===== BUTTONS =====
    float spacing = 60;
    float totalHeight = actionButtons.size() * spacing;

    float startY = y + h - totalHeight - 20; // bawah popup

    for (int i = 0; i < actionButtons.size(); i++) {
        actionButtons[i].movePosition({
            x + w/2,
            startY + i * spacing
        });

        actionButtons[i].render();
    }

    // EXIT BUTTON
    // exitButton.render();
}

StreetPopup::StreetPopup(
    const std::string& name,
    int buyPrice,
    int mortgageValue,
    const std::string& colorGroup,
    const std::vector<int>& rentTable,
    int baseRent,
    int buildCost,
    int level,
    bool colorGroupComplete,
    bool isOtherPlayer,
    const std::string& ownerName
)
: PropertyPopup(name, buyPrice, mortgageValue, isOtherPlayer, ownerName),
  colorGroup(colorGroup),
  rentTable(rentTable),
  baseRent(baseRent),
  buildCost(buildCost),
  level(level),
  colorGroupComplete(colorGroupComplete)
{
}

std::string StreetPopup::buildDetails() const {
    std::string s;

    // STATUS
    s += "Status: ";
    if (ownerName.empty()) s += "BANK";
    else s += "OWNED (" + ownerName + ")";
    s += "\n";

    s += "Type: STREET\n";
    s += "Color: " + colorGroup + "\n";

    // ===== BANK =====
    if (ownerName.empty()) {
        s += "Buy Price: " + std::to_string(buyPrice) + "\n";
        return s;
    }

    // ===== RENT CALC =====
    int rent = 0;

    if (level == 0) {
        rent = baseRent;

        if (colorGroupComplete) {
            rent *= 2; // monopoli tanpa bangunan
        }
    } else {
        if (!rentTable.empty()) {
            int idx = std::min(level, (int)rentTable.size() - 1);
            rent = rentTable[idx];
        }
    }

    s += "Rent: " + std::to_string(rent) + "\n";
    s += "Level: " + std::to_string(level) + "\n";

    // ===== OWNER ONLY INFO =====
    if (!isOtherPlayer) {
        s += "Build Cost: " + std::to_string(buildCost) + "\n";
        s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
    }

    return s;
}

RailroadPopup::RailroadPopup(
    const std::string& name,
    int buyPrice,
    int mortgageValue,
    const std::vector<int>& rentTable,
    int ownedCount,
    bool isOtherPlayer,
    const std::string& ownerName
)
: PropertyPopup(name, buyPrice, mortgageValue, isOtherPlayer, ownerName),
  rentTable(rentTable),
  ownedCount(ownedCount)
{
}

std::string RailroadPopup::buildDetails() const {
    std::string s;

    // STATUS
    s += "Status: ";
    if (ownerName.empty()) s += "BANK";
    else s += "OWNED (" + ownerName + ")";
    s += "\n";

    s += "Type: RAILROAD\n";

    // ===== BANK =====
    if (ownerName.empty()) {
        s += "Buy Price: " + std::to_string(buyPrice) + "\n";
        return s;
    }

    // ===== RENT =====
    int rent = 0;

    if (!rentTable.empty()) {
        int idx = std::clamp(ownedCount - 1, 0, (int)rentTable.size() - 1);
        rent = rentTable[idx];
    }

    s += "Owned: " + std::to_string(ownedCount) + "\n";
    s += "Rent: " + std::to_string(rent) + "\n";

    if (!isOtherPlayer) {
        s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
    }

    return s;
}

UtilityPopup::UtilityPopup(
    const std::string& name,
    int buyPrice,
    int mortgageValue,
    const std::vector<int>& multiplier,
    int ownedCount,
    int diceRoll,
    bool isOtherPlayer,
    const std::string& ownerName
)
: PropertyPopup(name, buyPrice, mortgageValue, isOtherPlayer, ownerName),
  multiplier(multiplier),
  ownedCount(ownedCount),
  lastDiceRoll(diceRoll)
{
}

std::string UtilityPopup::buildDetails() const {
    std::string s;

    // STATUS
    s += "Status: ";
    if (ownerName.empty()) s += "BANK";
    else s += "OWNED (" + ownerName + ")";
    s += "\n";

    s += "Type: UTILITY\n";

    // ===== BANK =====
    if (ownerName.empty()) {
        s += "Buy Price: " + std::to_string(buyPrice) + "\n";
        return s;
    }

    // ===== RENT =====
    int rent = 0;

    if (!multiplier.empty()) {
        int idx = std::clamp(ownedCount - 1, 0, (int)multiplier.size() - 1);
        rent = lastDiceRoll * multiplier[idx];
    }

    s += "Owned: " + std::to_string(ownedCount) + "\n";
    s += "Dice: " + std::to_string(lastDiceRoll) + "\n";
    s += "Rent: " + std::to_string(rent) + "\n";

    if (!isOtherPlayer) {
        s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
    }

    return s;
}

MessagePopup::MessagePopup(
    const std::string& title,
    const std::string& message,
    const std::string& imagePath
)
: IndefinitePopup(View2D(getScreenCenter(), {500, 350}, [](){})),
  title(title),
  message(message),
  hasImage(false)
{
    if (!imagePath.empty()) {
        if (!FileExists(imagePath.c_str())) {
            std::cout << "Image not found in " << imagePath << endl;
        } else {
            Image img = LoadImage(imagePath.c_str());
            // std::cout << "Image loaded: " << img.width << "x" << img.height << std::endl;

            texture = LoadTextureFromImage(img);
            // std::cout << "Texture loaded: " << texture.width << "x" << texture.height << std::endl;
            UnloadImage(img);
            hasImage = true;
        }
        imageSize = {150, 150};
    }
}

MessagePopup::~MessagePopup() {
    if (hasImage) {
        UnloadTexture(texture);
    }
}

void MessagePopup::enable() {
    exitButton.enable();
}

void MessagePopup::disable() {
    exitButton.disable();
}

void MessagePopup::interactionCheck() {
    exitButton.interactionCheck();
}

void MessagePopup::render() {
    animationCheck();

    float x = getRenderPos().x;
    float y = getRenderPos().y;
    float w = getRenderWidth();
    float h = getRenderHeight();

    // background
    DrawRectangle(x, y, w, h, RAYWHITE);

    // header
    float headerH = h * 0.2f;
    DrawRectangle(x, y, w, headerH, RED);

    // title
    Vector2 t = MeasureTextEx(fontMap["Orbitron"], title.c_str(), 26, 1);
    DrawTextEx(fontMap["Orbitron"], title.c_str(),
               {x + w/2 - t.x/2, y + headerH/2 - t.y/2},
               28, 1, WHITE);

    // content
    float currentY = y + headerH + 20; // MULAI DI BAWAH HEADER

    // image (jika ada)
    if (hasImage) {
        float scale = imageSize.x / texture.width;

        DrawTextureEx(texture, {x + w/2 - imageSize.x/2, currentY},
                      0.0f, scale, WHITE);

        currentY += imageSize.y + 20; // geser ke bawah setelah gambar
    }

    // text
    drawTextLinesWrapped(fontMap["Orbitron"], message,
                         {x + w/2, currentY},
                         20, 1, BLACK,
                         {w - 40, h}); // padding kiri kanan

    // exit button
    exitButton.render();
}

/*
Cara pakai StreetPopup, RailroadPopup, dan UtilityPopup (contoh di ViewTesting.cpp):
// STREET daru BANK
StreetPopup* s1 = new StreetPopup(
    "Bandung",        // name → nama properti
    200,              // buyPrice → harga beli dari bank
    100,              // mortgageValue → nilai gadai
    "YELLOW",         // colorGroup → kelompok warna
    {20,40,60,100,150,200}, // rentTable → sewa per level (1 rumah → hotel)
    20,               // baseRent → sewa dasar (tanpa bangunan)
    50,               // buildCost → biaya bangun rumah/hotel
    0,                // level → jumlah bangunan (0 = kosong)
    false,            // colorGroupComplete → belum monopoli
    false,            // isOtherPlayer → irrelevant (BANK)
    ""                // ownerName → kosong = BANK
);

// tombol
s1->addButton("BUY", "BUY_PROPERTY");

//STREET milik sendiri
StreetPopup* s2 = new StreetPopup(
    "Medan",
    220,
    110,
    "RED",
    {30,60,90,150,220,300},
    30,
    75,
    2,          // level → 2 rumah
    true,       // monopoli
    false,      // milik sendiri
    "Player1"
);

// tombol
s2->addButton("BUILD", "BUILD_HOUSE");
s2->addButton("MORTGAGE", "MORTGAGE_PROPERTY");

//STREET milik orang lain
StreetPopup* s3 = new StreetPopup(
    "Jakarta",
    300,
    150,
    "BLUE",
    {50,100,150,250,400,600},
    50,
    100,
    3,          // 3 rumah
    true,       // monopoli
    true,       // milik orang lain
    "Player2"
);

// tombol
s3->addButton("PAY RENT", "PAY_RENT");

// RAILROAD miliki BANK
RailroadPopup* r1 = new RailroadPopup(
    "Stasiun Utara",     // name
    200,                 // buyPrice
    100,                 // mortgageValue
    {25,50,100,200},     // rentTable → tergantung jumlah railroad
    0,                   // ownedCount → belum ada yang punya
    false,
    ""
);

r1->addButton("BUY", "BUY_PROPERTY");

// RAILROAD milik sendiri
RailroadPopup* r2 = new RailroadPopup(
    "Stasiun Barat",
    200,
    100,
    {25,50,100,200},
    2,              // punya 2 railroad
    false,
    "Player1"
);

r2->addButton("MORTGAGE", "MORTGAGE_PROPERTY");

// RAILROAD milik orang lain
RailroadPopup* r3 = new RailroadPopup(
    "Stasiun Timur",
    200,
    100,
    {25,50,100,200},
    3,              // owner punya 3 railroad
    true,
    "Player2"
);

r3->addButton("PAY RENT", "PAY_RENT");

// UTILITY milik BANK
UtilityPopup* u1 = new UtilityPopup(
    "PLN",
    150,
    75,
    {4,10},     // multiplier → 1 utility: x4, 2 utility: x10
    0,
    0,
    false,
    ""
);

u1->addButton("BUY", "BUY_PROPERTY");

// UTILITY milik sendiri
UtilityPopup* u2 = new UtilityPopup(
    "PDAM",
    150,
    75,
    {4,10},
    2,          // punya 2 utility
    7,          // lastDiceRoll (opsional info)
    false,
    "Player1"
);

u2->addButton("MORTGAGE", "MORTGAGE_PROPERTY");

// UTILITY milik orang lain
UtilityPopup* u3 = new UtilityPopup(
    "Internet",
    150,
    75,
    {4,10},
    1,          // owner punya 1 utility
    8,          // hasil dadu pemain
    true,
    "Player2"
);

u3->addButton("PAY RENT", "PAY_RENT");

Cara pakai MessagePopup (contoh di ViewTesting.cpp)
// Dengan Gambar
 app.loadPopup(new MessagePopup(
    "Festival Tile",
    "Festival is here! Rent for all properties is doubled for 3 turns!",
    "src/views/Festival1.png"
));

//Tanpa Gambar
app.loadPopup(new MessagePopup(
    "Purchase Property",
    "Property successfully purchased!"
));
*/