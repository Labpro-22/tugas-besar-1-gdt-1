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
    confirmButton(Interactable({320, 50}, true, false, "LOAD_GAME " + filePath, [](){}, [this](){ this->closeView = true;})) 
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

string LoadConfirmPopup::catchCommand() {
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

string ExceptionPopup::catchCommand() {
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
                  
    std::string fullText = "Code: " + std::to_string(errorCode) + "\n \n" + errorMessage;
    drawTextLinesWrapped(fontMap["Orbitron"], fullText, pos, 
                         getRenderFontSize(28), 1, getRenderColor(BLACK), getRenderDim() - (Vector2){20, 0});
                         
    okButton.render();
}

PropertyPopup::PropertyPopup(
    const std::string& name,
    const std::string& type,
    const std::string& status,
    int buyPrice,
    int mortgageValue,
    int levelOrCount,
    bool isOtherPlayer,
    const std::string& ownerName,
    const std::string& colorGroup,
    const std::vector<int>& rentTable,
    int buildCost,
    const std::vector<int>& railroadRent,
    const std::vector<int>& utilityMultiplier
)
: IndefinitePopup(View2D(getScreenCenter(), {520, 420}, [](){})),
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
        this->pos.y + this->getRenderDim().y/2 - 60
    };

    // BUTTON LOGIC
    // BANK
    if (status == "BANK") {
        if (type == "STREET") {
            addButton("BUY", "BUY_PROPERTY");
            addButton("SKIP", "SKIP_PROPERTY");
        } else {
            addButton("OK", "ACK");
        }
    }

    // OWNED
    else if (status == "OWNED") {

        if (isOtherPlayer) {
            addButton("PAY", "PAY_RENT");
        } 
        else {
            // milik sendiri

            if (type == "STREET") {
                if (levelOrCount == 0) {
                    addButton("BUILD", "BUILD_PROPERTY");
                    addButton("MORTGAGE", "MORTGAGE_PROPERTY");
                } 
                else if (levelOrCount < 5) {
                    addButton("BUILD", "BUILD_PROPERTY");
                }
            } 
            else {
                addButton("MORTGAGE", "MORTGAGE_PROPERTY");
            }

            addButton("OK", "ACK");
        }
    }

    // MORTGAGED
    else if (status == "MORTGAGED") {
        addButton("REDEEM", "REDEEM_PROPERTY");
        addButton("OK", "ACK");
    }

    float startY = this->pos.y + this->getRenderDim().y/2 - 60;

    for (int i = 0; i < actionButtons.size(); i++) {
        actionButtons[i].movePosition({
            this->pos.x,
            startY - i * 70
        });

        actionButtons[i].setRender([this, i]() {
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
                    WHITE);
        });
    }
    std::cout << "PropertyPopup constructed\n";
}

void PropertyPopup::enable() {
    for (auto& btn : actionButtons) btn.enable();
    exitButton.enable();
}

void PropertyPopup::disable() {
    for (auto& btn : actionButtons) btn.disable();
    exitButton.disable();
}

void PropertyPopup::addButton(const std::string& label, const std::string& command) {
    Interactable btn(
        {220, 50},
        true,
        false,
        label,
        [](){},
        [this, command]() {
            this->actionCommand = command;
            this->closeView = true;
        }
    );

    actionButtons.push_back(btn);
}

void PropertyPopup::interactionCheck() {
    for (auto& btn : actionButtons) btn.interactionCheck();
    exitButton.interactionCheck();
}

string PropertyPopup::catchCommand() {
    if (actionCommand != "") return actionCommand;

    for (auto& btn : actionButtons) {
        string cmd = btn.catchCommand();
        if (cmd != "NULL") return cmd;
    }

    return "NULL";
}

// Helper
std::string PropertyPopup::buildDetails() const {
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
    if (type == "STREET") {
        s += "Color: " + colorGroup + "\n";

        if (status == "BANK") {
            s += "Buy Price: " + std::to_string(buyPrice) + "\n";
            return s;
        }

        if (status == "OWNED") {
            if (!rentTable.empty()) {
                int idx = std::max(0, levelOrCount);
                idx = std::min(idx, (int)rentTable.size() - 1);
                int rent = rentTable[idx];
                s += "Current Rent: " + std::to_string(rent) + "\n";
            }

            if (!isOtherPlayer) {
                s += "Build Cost: " + std::to_string(buildCost) + "\n";
                s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
                s += "Level: " + std::to_string(levelOrCount) + "\n";
            }
        }

        if (status == "MORTGAGED") {
            s += "This property is mortgaged.\n";
            if (!isOtherPlayer) {
                s += "Mortgage Value: " + std::to_string(mortgageValue) + "\n";
            }
        }
        // std::cout << "Building details street\n";
    }

    // RAILROAD
    else if (type == "RAILROAD") {
        if (status == "OWNED") {
            if (!railroadRent.empty()) {
                int idx = std::max(0, levelOrCount - 1);
                idx = std::min(idx, (int)railroadRent.size() - 1);
                int rent = railroadRent[idx];
                s += "Rent: " + std::to_string(rent) + "\n";
            }

            s += "Owned Count: " + std::to_string(levelOrCount) + "\n";

            if (!isOtherPlayer) {
                s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
            }
        }

        else if (status == "MORTGAGED") {
            s += "This railroad is mortgaged.\n";

            if (!isOtherPlayer) {
                s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
            }
        }
        // std::cout << "Building details railroad\n";
    }
    
    // UTILITY
    else if (type == "UTILITY") {
        if (status == "OWNED") {
            if (!utilityMultiplier.empty()) {
                int idx = std::max(0, levelOrCount - 1);
                idx = std::min(idx, (int)utilityMultiplier.size() - 1);
                int mult = utilityMultiplier[idx];
                s += "Multiplier: x" + std::to_string(mult) + "\n";
            }

            s += "Owned Count: " + std::to_string(levelOrCount) + "\n";

            if (!isOtherPlayer) {
                s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
            }
        }

        else if (status == "MORTGAGED") {
            s += "This utility is mortgaged.\n";

            if (!isOtherPlayer) {
                s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
            }
        }
        // std::cout << "Building details utility\n";
    }

    return s;
}

void PropertyPopup::render() {
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
               pos - Vector2{t.x/2, getRenderHeight()/2 - 10},
               28,1, WHITE);
    // std::cout << "Setting up title\n";

    // details
    std::string text = buildDetails();

    // menghapus baris kosong
    std::stringstream ss(text);
    std::string line;
    std::string fixedText;

    while (std::getline(ss, line)) {
        if (!line.empty()) {
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
        getRenderDim() - (Vector2){40,0}
    );
    // std::cout << "Drawing text lines\n";

    for (auto& btn : actionButtons) btn.render();
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