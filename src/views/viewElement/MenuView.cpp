#include "views/viewElement/MenuView.hpp"


MainMenuView::MainMenuView() : 
    MenuView(View2D(getScreenCenter(), 
                    {(float)GetScreenWidth(), (float)GetScreenHeight()}, [](){})),
    startButton(Interactable({500, 50}, true, false, "START GAME", [](){}, [](){})),
    exitButton(Interactable({500, 50}, true, false, "CLOSE GAME", [](){}, [](){})),
    loadFileEntry(Entry({500, 50}, "Enter savefile path", 24, "Orbitron", [](){})) {
    Vector2 screenCenter = getScreenCenter();
    startButton.movePosition(screenCenter + (Vector2){0.0f, 100.0f});
    startButton.setRender([this](){
        DrawRectangle(this->startButton.getRenderPos().x, this->startButton.getRenderPos().y,
                      this->startButton.getRenderWidth(), this->startButton.getRenderHeight(), 
                      this->startButton.getRenderColor(VIOLET));
        Vector2 textDim = MeasureTextEx(fontMap.at("Orbitron"), "Start Game", this->startButton.getRenderFontSize(36), 0);
        DrawTextEx(fontMap.at("Orbitron"), "Start Game", {this->startButton.getX() - textDim.x/2,
                   this->startButton.getY() - 18}, this->startButton.getRenderFontSize(36), 0, this->startButton.getRenderColor(BLACK));
    });
    
    loadFileEntry.movePosition(screenCenter + (Vector2){0.0f, 170.f});
    loadFileEntry.setOnEnterFunc([this](){
        this->loadFileEntry.setGameCommand("DISPLAY LOAD_CONFIRM_POPUP " + loadFileEntry.getEntryText());
    });

    exitButton.movePosition(screenCenter + (Vector2){0.0f, 240.0f});
    exitButton.setRender([this](){
        DrawRectangle(this->exitButton.getRenderPos().x, this->exitButton.getRenderPos().y,
                      this->exitButton.getRenderWidth(), this->exitButton.getRenderHeight(), 
                      this->exitButton.getRenderColor(VIOLET));
        Vector2 textDim = MeasureTextEx(fontMap.at("Orbitron"), "Exit Game", this->exitButton.getRenderFontSize(36), 0);
        DrawTextEx(fontMap.at("Orbitron"), "Exit Game", 
                  {this->exitButton.getX() - textDim.x/2, this->exitButton.getY() - 18}, this->exitButton.getRenderFontSize(36), 0, 
                  this->exitButton.getRenderColor(BLACK));
    });
}


void MainMenuView::render() {
    DrawRectangle(getRenderPos().x, getRenderPos().y, boundingDim.x, boundingDim.y, DARKGRAY);
    startButton.render();
    loadFileEntry.render();
    exitButton.render();
}

void MainMenuView::interactionCheck() {
    startButton.interactionCheck();
    loadFileEntry.interactionCheck();
    exitButton.interactionCheck();
}

const string MainMenuView::catchCommand() {
    string startCommand = startButton.catchCommand();
    string exitCommand = exitButton.catchCommand();
    string loadCommand = loadFileEntry.catchCommand();
    if (startCommand != "NULL") {
        return startCommand;
    }
    if (exitCommand != "NULL") {
        return exitCommand;
    }
    if (loadCommand!= "NULL") {
        return loadCommand;
    }
    return "NULL";
}

void MainMenuView::enable() {
    startButton.enable();
    exitButton.enable();
    loadFileEntry.enable();
}

void MainMenuView::disable() {
    startButton.disable();
    exitButton.disable();
    loadFileEntry.disable();
}


