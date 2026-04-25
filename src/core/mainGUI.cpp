#include "core/GameEngine.hpp"
#include "views/IGUI.hpp"
#include "views/viewElement/View2D.hpp"
#include "views/GUI2.hpp"
#include "raylib.h"

static constexpr int SCREEN_W = 1280;
static constexpr int SCREEN_H = 720;
static constexpr int TARGET_FPS = 60;
static const char *WINDOW_TITLE = "Monopoly";

static void loadFonts()
{
    View2D::addFont("Orbitron", "data/GUIAssets/fonts/Orbitron.ttf");
    View2D::addFont("Kabel", "data/GUIAssets/kabel.ttf");
    // tambah font lain di sini jika ada
}

int main()
{
    InitWindow(SCREEN_W, SCREEN_H, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);

    loadFonts();

    ConfigLoader *CL = new ConfigLoader("data/config/default");
    GameConfig GC = CL->loadGameConfig();
    Board *b = CL->buildBoard(GC.getProperties(), GC);

    IGUI *gui = new GUI(static_cast<float>(TARGET_FPS), *b);
    GameEngine engine(gui);
    engine.run();

    View2D::unloadFonts(); 
    CloseWindow();

    delete gui;
    return 0;
}