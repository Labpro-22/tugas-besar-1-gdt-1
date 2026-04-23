#include "views/viewElement/MenuView.hpp"
#include "../include/views/animation/ViewAnimation.hpp"

MainMenuView::MainMenuView() : MenuView(View2D(getScreenCenter(),
                                               {(float)GetScreenWidth(), (float)GetScreenHeight()}, []() {})),
                               startButton(Interactable({500, 60}, true, false, "NEW_GAME", []() {}, []() {})), loadButton(Interactable({500, 60}, true, false, "LOAD_GAME", []() {}, []() {})), exitButton(Interactable({500, 60}, true, false, "EXIT", []() {}, []() {}))
{
    Vector2 screenCenter = getScreenCenter();

    // NEW GAME BUTTON
    startButton.movePosition(screenCenter + (Vector2){0.0f, 80.0f});
    startButton.setRender([this]()
                          {
        DrawRectangle(this->startButton.getRenderPos().x, this->startButton.getRenderPos().y,
                      this->startButton.getRenderWidth(), this->startButton.getRenderHeight(),
                      this->startButton.getRenderColor(LOGO_RED));

        Vector2 textDim = MeasureTextEx(fontMap.at("Orbitron"), "New Game",
                                        this->startButton.getRenderFontSize(36), 0);

        DrawTextEx(fontMap.at("Orbitron"), "New Game",
                   {this->startButton.getX() - textDim.x/2,
                    this->startButton.getY() - 18},
                   this->startButton.getRenderFontSize(36), 0,
                   this->startButton.getRenderColor(WHITE)); });

    // LOAD GAME BUTTON
    loadButton.movePosition(screenCenter + (Vector2){0.0f, 160.0f});
    loadButton.setRender([this]()
                         {
        DrawRectangle(this->loadButton.getRenderPos().x, this->loadButton.getRenderPos().y,
                      this->loadButton.getRenderWidth(), this->loadButton.getRenderHeight(),
                      this->loadButton.getRenderColor(LOGO_RED));

        Vector2 textDim = MeasureTextEx(fontMap.at("Orbitron"), "Load Game",
                                        this->loadButton.getRenderFontSize(36), 0);

        DrawTextEx(fontMap.at("Orbitron"), "Load Game",
                   {this->loadButton.getX() - textDim.x/2,
                    this->loadButton.getY() - 18},
                   this->loadButton.getRenderFontSize(36), 0,
                   this->loadButton.getRenderColor(WHITE)); });

    // EXIT BUTTON
    exitButton.movePosition(screenCenter + (Vector2){0.0f, 240.0f});
    exitButton.setRender([this]()
                         {
        DrawRectangle(this->exitButton.getRenderPos().x, this->exitButton.getRenderPos().y,
                      this->exitButton.getRenderWidth(), this->exitButton.getRenderHeight(),
                      this->exitButton.getRenderColor(LOGO_RED));

        Vector2 textDim = MeasureTextEx(fontMap.at("Orbitron"), "Exit",
                                        this->exitButton.getRenderFontSize(36), 0);

        DrawTextEx(fontMap.at("Orbitron"), "Exit",
                   {this->exitButton.getX() - textDim.x/2,
                    this->exitButton.getY() - 18},
                   this->exitButton.getRenderFontSize(36), 0,
                   this->exitButton.getRenderColor(WHITE)); });

    // ANIMATION
    ViewAnimation *anim = new ViewAnimation(*this, 120, true, []() {}, [this]()
                                            { closeView = true; });

    anim->setMoveAnimation({pos.x, pos.y - getRenderHeight()}, 1);
    this->addAnimation("START", anim);

    // LOGO
    Image img = LoadImage("data/GUIAssets/nimonspoli_logo.png");
    logo = LoadTextureFromImage(img);
    UnloadImage(img);
}

MainMenuView::~MainMenuView()
{
    UnloadTexture(logo);
}

void MainMenuView::movePosition(const Vector2 &v)
{
    Vector2 delta = v - pos;
    pos = v;

    startButton.movePositionDelta(delta);
    loadButton.movePositionDelta(delta);
    exitButton.movePositionDelta(delta);
}

void MainMenuView::render()
{
    animationCheck();

    Vector2 center = {
        (float)GetScreenWidth() / 2,
        (float)GetScreenHeight() / 2};

    startButton.setPosition(center + Vector2{0, 80});
    loadButton.setPosition(center + Vector2{0, 160});
    exitButton.setPosition(center + Vector2{0, 240});

    // background
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {100, 100, 100, 150});

    // logo
    DrawTexture(logo,
                center.x - logo.width / 2,
                center.y - logo.height / 2 - 220,
                WHITE);

    startButton.render();
    loadButton.render();
    exitButton.render();
}

void MainMenuView::interactionCheck()
{
    startButton.interactionCheck();
    loadButton.interactionCheck();
    exitButton.interactionCheck();
}

std::string MainMenuView::catchCommand()
{
    std::string cmd;

    cmd = startButton.catchCommand();
    if (cmd != "NULL")
        return cmd;

    cmd = loadButton.catchCommand();
    if (cmd != "NULL")
        return cmd;

    cmd = exitButton.catchCommand();
    if (cmd != "NULL")
        return cmd;

    return "NULL";
}

void MainMenuView::enable()
{
    startButton.enable();
    loadButton.enable();
    exitButton.enable();
}

void MainMenuView::disable()
{
    startButton.disable();
    loadButton.disable();
    exitButton.disable();
}