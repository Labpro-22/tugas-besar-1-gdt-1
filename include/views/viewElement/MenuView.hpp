#pragma once
#include "View2D.hpp"
#include "Interactable.hpp"

class MenuView : public View2D
{
public:
    MenuView() : View2D() {}
    MenuView(const View2D &view) : View2D(view) {}
};

class MainMenuView : public MenuView
{
private:
    Interactable startButton;
    Interactable loadButton;
    Interactable exitButton;
    Texture2D logo;

public:
    MainMenuView();
    ~MainMenuView();
    void movePosition(const Vector2 &v) override;
    void enable() override;
    void disable() override;
    void interactionCheck() override;
    std::string catchCommand() override;
    void render() override;
};

class GameFinishMenuView : public MenuView
{
private:
    Interactable playAgainButton;
    Interactable returnToMainMenuButton;
};
