#include "Popup.hpp"


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