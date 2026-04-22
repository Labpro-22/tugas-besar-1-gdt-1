#include "core/GameEngine.hpp"
#include "views/GUI.hpp"
#include "../include/utils/data/GameConfig.hpp"
#include "../include/utils/data/ConfigLoader.hpp"
#include "exception/GameException.hpp"

int main()
{
    const int screenWidth = 1200;
    const int screenHeight = 800;
    SetTraceLogLevel(LOG_NONE);
    InitWindow(screenWidth, screenHeight, "Nimonspoli");
    ClearWindowState(FLAG_WINDOW_RESIZABLE);
    View2D::addFont("Kabel", "data/GUIAssets/kabel.ttf");
    View2D::addFont("Orbitron", "data/GUIAssets/Orbitron-VariableFont_wght.ttf");
    SetTargetFPS(120);

    try
    {
        ConfigLoader *CL = new ConfigLoader("data/default");
        GameConfig GC = CL->loadGameConfig();
        Board *b = CL->buildBoard(GC.getProperties(), GC);

        GUI app(120, *b);
        GameEngine engine(&app);
        app.loadMainMenu();
        app.loadDebuggingEntry();

        while (!WindowShouldClose())
        {
            try
            {
                engine.update();
            }
            catch (const GameException &e)
            {
                app.loadPopup(new ExceptionPopup(e.getErrorCode(), e.what()));
            }
            catch (const std::exception &e)
            {
                app.loadPopup(new ExceptionPopup(500, e.what()));
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            app.display();
            DrawFPS(10, 10);
            EndDrawing();
        }

        delete CL;
        delete b;
    }
    catch (const std::exception &e)
    {
        std::cerr << "FATAL INIT ERROR: " << e.what() << std::endl;
    }

    View2D::unloadFonts();
    CloseWindow();

    return 0;
}