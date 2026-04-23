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

// ===== PropertyPopup =====
PropertyPopup::PropertyPopup(Property* property, bool isOtherPlayer)
    : IndefinitePopup(View2D(getScreenCenter(), {520, 420}, [](){})),
      property(property), isOtherPlayer(isOtherPlayer) {}

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

    // BACKGROUND
    DrawRectangle(x, y, w, h, RAYWHITE);

    // HEADER
    float headerH = h * 0.2f;
    DrawRectangle(x, y, w, headerH, RED);

    // TITLE
    Vector2 t = MeasureTextEx(fontMap["Orbitron"], property->getName().c_str(), 26, 1);
    DrawTextEx(
        fontMap["Orbitron"],
        property->getName().c_str(),
        {x + w/2 - t.x/2, y + headerH/2 - t.y/2},
        26,
        1,
        WHITE
    );

    // DETAILS (DI BAWAH HEADER)
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

    // BUTTONS
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
}

// ===== StreetPopup =====
StreetPopup::StreetPopup(
    StreetProperty* street,
    bool isOtherPlayer,
    bool colorGroupComplete
)
: PropertyPopup(street, isOtherPlayer),
  colorGroupComplete(colorGroupComplete) {}

std::string StreetPopup::buildDetails() const {
    auto* street = static_cast<StreetProperty*>(property);
    std::string s;

    // STATUS
    s += "Status: ";
    std::string ownerName = property->getOwner() ? property->getOwner()->getUsername() : "";
    if (ownerName.empty()) s += "BANK";
    else s += "OWNED (" + ownerName + ")";
    s += "\n";

    s += "Type: STREET\n";
    s += "Color: " + street->getColorGroup() + "\n";

    // BANK
    if (ownerName.empty()) {
        s += "Buy Price: " + std::to_string(property->getPurchasePrice()) + "\n";
        return s;
    }

    // RENT
    int rent = property->calculateRent();
    if (street->getBuildingState() == BuildingState::NONE && colorGroupComplete) {
        rent *= 2;
    }

    s += "Rent: " + std::to_string(rent) + "\n";
    s += "Building: " + std::to_string((int)street->getBuildingState()) + "\n";

    if (!isOtherPlayer) {
        s += "Build Cost: " + std::to_string(street->getHouseBuildCost()) + "\n";
        s += "Mortgage: " + std::to_string(property->getMortgageValue()) + "\n";
    }

    return s;
}

// ===== RailroadPopup =====
RailroadPopup::RailroadPopup(
    RailroadProperty* railroad,
    bool isOtherPlayer,
    int ownedCount
)
: PropertyPopup(railroad, isOtherPlayer),
  ownedCount(ownedCount) {}

std::string RailroadPopup::buildDetails() const {
    std::string s;

    std::string ownerName = property->getOwner() ? property->getOwner()->getUsername() : "";

    s += "Status: ";
    if (ownerName.empty()) s += "BANK";
    else s += "OWNED (" + ownerName + ")";
    s += "\n";

    s += "Type: RAILROAD\n";

    if (ownerName.empty()) {
        s += "Buy Price: " + std::to_string(property->getPurchasePrice()) + "\n";
        return s;
    }

    int rent = property->calculateRent();
    s += "Owned: " + std::to_string(ownedCount) + "\n";
    s += "Rent: " + std::to_string(rent) + "\n";

    if (!isOtherPlayer) {
        s += "Mortgage: " + std::to_string(property->getMortgageValue()) + "\n";
    }

    return s;
}

// ===== UtilityPopup =====
UtilityPopup::UtilityPopup(
    UtilityProperty* utility,
    bool isOtherPlayer,
    int ownedCount,
    int diceRoll
)
: PropertyPopup(utility, isOtherPlayer),
  ownedCount(ownedCount),
  lastDiceRoll(diceRoll) {}

std::string UtilityPopup::buildDetails() const {
    std::string s;

    std::string ownerName = property->getOwner() ? property->getOwner()->getUsername() : "";

    s += "Status: ";
    if (ownerName.empty()) s += "BANK";
    else s += "OWNED (" + ownerName + ")";
    s += "\n";

    s += "Type: UTILITY\n";

    if (ownerName.empty()) {
        s += "Buy Price: " + std::to_string(property->getPurchasePrice()) + "\n";
        return s;
    }

    int rent = property->calculateRent(lastDiceRoll);
    s += "Owned: " + std::to_string(ownedCount) + "\n";
    s += "Dice: " + std::to_string(lastDiceRoll) + "\n";
    s += "Rent: " + std::to_string(rent) + "\n";

    if (!isOtherPlayer) {
        s += "Mortgage: " + std::to_string(property->getMortgageValue()) + "\n";
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
// ===== STREET =====
StreetProperty* s = new StreetProperty(
    "BDG",                          // code
    "Bandung",                      // name
    300,                            // purchasePrice
    150,                            // mortgageValue
    "HIJAU",                        // colorGroup
    200,                            // houseBuildCost
    200,                            // hotelBuildCost
    {26, 130, 390, 900, 1100, 1275} // rentLevels (L0 - hotel)
);

StreetPopup* sp = new StreetPopup(s, false, false);
sp->addButton("BUY", "BUY_PROPERTY");
app.loadPopup(sp);

// ===== RAILROAD =====
RailroadProperty* r = new RailroadProperty(
    "GBR",                          // code
    "Stasiun Gambir",               // name
    200,                            // purchasePrice
    100,                            // mortgageValue
    {{1, 25}, {2, 50}, {3, 100}, {4, 200}} // rentTable
);

RailroadPopup* rp = new RailroadPopup(r, false, 1);
rp->addButton("BUY", "BUY_PROPERTY");
app.loadPopup(rp);

// ===== UTILITY =====
UtilityProperty* u = new UtilityProperty(
    "PLN",                          // code
    "PLN",                          // name
    150,                            // purchasePrice
    75,                             // mortgageValue
    {{1, 4}, {2, 10}}               // multiplierTable
);

UtilityPopup* up = new UtilityPopup(u, false, 1, 7);
up->addButton("BUY", "BUY_PROPERTY");
app.loadPopup(up);

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