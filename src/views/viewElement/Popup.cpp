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
    Type type,
    Status status,
    int buyPrice,
    int mortgageValue,
    bool isOtherPlayer,
    const std::string& ownerName
)
: IndefinitePopup(View2D(getScreenCenter(), {520, 420}, [](){})),
  name(name),
  type(type),
  status(status),
  buyPrice(buyPrice),
  mortgageValue(mortgageValue),
  isOtherPlayer(isOtherPlayer),
  ownerName(ownerName),
  actionCommand("")
{
}

void PropertyPopup::setStreetData(
    const std::string& colorGroup,
    const std::vector<int>& rentTable,
    int baseRent,
    int buildCost,
    int level,
    bool colorGroupComplete
) {
    streetData = StreetData{
        colorGroup,
        rentTable,
        baseRent,
        buildCost,
        level,
        colorGroupComplete
    };
}

void PropertyPopup::setRailroadData(
    const std::vector<int>& rentTable,
    int ownedCount
) {
    railroadData = RailroadData{
        rentTable,
        ownedCount
    };
}

void PropertyPopup::setUtilityData(
    const std::vector<int>& multiplier,
    int ownedCount,
    int diceRoll
) {
    utilityData = UtilityData{
        multiplier,
        ownedCount,
        diceRoll
    };
}

std::string PropertyPopup::buildDetails() const {
    std::string s;

    // STATUS
    s += "Status: ";
    if (status == Status::BANK) s += "BANK";
    else if (status == Status::OWNED) s += "OWNED";
    else s += "MORTGAGED";

    if (status == Status::OWNED && !ownerName.empty()) {
        s += " (" + ownerName + ")";
    }
    s += "\n\n";

    // TYPE
    if (type == Type::STREET) s += "Type: STREET\n";
    else if (type == Type::RAILROAD) s += "Type: RAILROAD\n";
    else s += "Type: UTILITY\n";

    // STREET
    if (type == Type::STREET && streetData.has_value()) {
        const auto& d = streetData.value();

        s += "Color: " + d.colorGroup + "\n";

        // BANK --> hanya info beli
        if (status == Status::BANK) {
            s += "Buy Price: " + std::to_string(buyPrice) + "\n";
            return s;
        }

        // RENT LOGIC
        int rent = 0;

        if (status != Status::MORTGAGED) {
            if (d.level == 0) {
                rent = d.baseRent;

                // MONOPOLI TANPA BANGUNAN
                if (d.colorGroupComplete) {
                    rent *= 2;
                }
            } else {
                int idx = std::min(d.level, (int)d.rentTable.size() - 1);
                rent = d.rentTable[idx];
            }

            rent = (int)(rent * d.festivalMultiplier);
        }

        s += "Rent: " + std::to_string(rent) + "\n";
        s += "Level: " + std::to_string(d.level) + "\n";

        if (!isOtherPlayer) {
            if (status == Status::OWNED) {
                if (d.level > 0) {
                    s += "Cannot Mortgage (has buildings)\n";
                } else {
                    s += "Can Mortgage\n";
                }
            }

            if (status == Status::MORTGAGED) {
                s += "Mortgaged\n";
                s += "Redeem Cost: " + std::to_string(buyPrice) + "\n";
            }

            s += "Mortgage Value: " + std::to_string(mortgageValue) + "\n";
        }
    }

    // RAILROAD
    else if (type == Type::RAILROAD && railroadData.has_value()) {
        const auto& d = railroadData.value();

        int rent = 0;

        if (status != Status::MORTGAGED && !d.rentTable.empty()) {
            int idx = std::max(0, d.ownedCount - 1);
            idx = std::min(idx, (int)d.rentTable.size() - 1);
            rent = d.rentTable[idx];
        }

        s += "Owned: " + std::to_string(d.ownedCount) + "\n";
        s += "Rent: " + std::to_string(rent) + "\n";

        if (!isOtherPlayer) {
            s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
        }
    }

    // UTILITY
    else if (type == Type::UTILITY && utilityData.has_value()) {
        const auto& d = utilityData.value();

        int rent = 0;

        if (status != Status::MORTGAGED && !d.multiplier.empty()) {
            int idx = std::max(0, d.ownedCount - 1);
            idx = std::min(idx, (int)d.multiplier.size() - 1);

            rent = d.lastDiceRoll * d.multiplier[idx];
        }

        s += "Owned: " + std::to_string(d.ownedCount) + "\n";
        s += "Dice: " + std::to_string(d.lastDiceRoll) + "\n";
        s += "Rent: " + std::to_string(rent) + "\n";

        if (!isOtherPlayer) {
            s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
        }
    }

    return s;
}

void PropertyPopup::addButton(const std::string& label, const std::string& command) {
    // buat tombol
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

    // ambil index tombol terakhir
    int idx = actionButtons.size() - 1;

    // set render
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
    return actionCommand.empty() ? "NULL" : actionCommand;
}

void PropertyPopup::render() {
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
    Vector2 t = MeasureTextEx(fontMap["Orbitron"], name.c_str(), 26, 1);
    DrawTextEx(fontMap["Orbitron"], name.c_str(),
               {x + w/2 - t.x/2, y + headerH/2 - t.y/2},
               28, 1, WHITE);

    // details
    float currentY = y + headerH + 20; // MULAI DI BAWAH HEADER

    std::string text = buildDetails();

    std::stringstream ss(text);
    std::string line;
    std::string fixedText;

    while (std::getline(ss, line)) {
        if (!line.empty()) {   //BUANG BARIS KOSONG
            fixedText += line + "\n";
        }
    }

    drawTextLinesWrapped(fontMap["Orbitron"], fixedText,
                         {x + w/2, currentY+ 60},
                         20, 1, BLACK,
                         {w - 40, h}); // padding kiri kanan

    // buttons layout
    float spacing = 60;
    float totalHeight = actionButtons.size() * spacing;
    float startY = pos.y + getRenderDim().y/2 - totalHeight - 20;

    for (int i = 0; i < actionButtons.size(); i++) {
        actionButtons[i].movePosition({
            pos.x,
            startY + i * spacing
        });

        actionButtons[i].render();
    }

    // exitButton.render();
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
Cara pakai PropertyPopup (contoh di ViewTesting.cpp):
// STREET EXAMPLE
PropertyPopup* streetPopup = new PropertyPopup(
    "Medan",                          // nama properti
    PropertyPopup::Type::STREET,      // tipe properti (STREET)
    PropertyPopup::Status::OWNED,     // status (BANK / OWNED / MORTGAGED)
    200,                              // buyPrice --> harga beli properti
    100,                              // mortgageValue --> uang yang didapat saat digadai
    true,                             // isOtherPlayer --> milik pemain lain?
    "Player2"                         // ownerName --> nama pemilik
);

// isi data khusus STREET
streetPopup->setStreetData(
    "YELLOW",                         // colorGroup --> grup warna
    {20, 40, 60, 100, 150, 200},      // rentTable --> sewa per level (0–5)
    20,                               // baseRent --> sewa dasar (level 0)
    50,                               // buildCost --> harga bangun rumah/hotel
    2,                                // level --> jumlah rumah (0–4), 5 = hotel
    true                              // colorGroupComplete --> apakah monopoli?
);

// tombol (karena milik orang lain --> bayar)
streetPopup->addButton("PAY RENT", "PAY_RENT");

app.loadPopup(streetPopup);

// RAILROAD EXAMPLE
PropertyPopup* railroadPopup = new PropertyPopup(
    "Gambir Station",                 // nama properti
    PropertyPopup::Type::RAILROAD,    // tipe RAILROAD
    PropertyPopup::Status::OWNED,     // status
    200,                              // buyPrice
    100,                              // mortgageValue
    true,                             // milik orang lain
    "Player1"                         // nama owner
);

// isi data railroad
railroadPopup->setRailroadData(
    {25, 50, 100, 200},               // rentTable --> berdasarkan jumlah railroad dimiliki
    3                                 // ownedCount --> jumlah railroad dimiliki owner
);

// tombol
railroadPopup->addButton("PAY RENT", "PAY_RENT");

app.loadPopup(railroadPopup);

// UTILITY EXAMPLE
PropertyPopup* utilityPopup = new PropertyPopup(
    "PLN",                            // nama properti
    PropertyPopup::Type::UTILITY,     // tipe UTILITY
    PropertyPopup::Status::OWNED,     // status
    150,                              // buyPrice
    75,                               // mortgageValue
    true,                             // milik orang lain
    "Player3"                         // owner
);

// isi data utility
utilityPopup->setUtilityData(
    {4, 10},                          // multiplier --> 1 utility = x4, 2 utility = x10
    2,                                // ownedCount --> jumlah utility dimiliki
    8                                 // diceRoll --> hasil dadu player yang mendarat
);

// tombol
utilityPopup->addButton("PAY RENT", "PAY_RENT");

app.loadPopup(utilityPopup);

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