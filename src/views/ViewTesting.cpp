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
        ConfigLoader CL("data/default");
        GameConfig GC = CL.loadGameConfig();

        Board &b = *CL.buildBoard(GC.getProperties(), GC);
        GUI app(120, b);
        app.loadMainMenu();
        app.loadDebuggingEntry();

        // Test Popup untuk Property
        // StreetPopup* s2 = new StreetPopup(
        //     "Medan",
        //     220,
        //     110,
        //     "RED",
        //     {30,60,90,150,220,300},
        //     30,
        //     75,
        //     2,          // level → 2 rumah
        //     true,       // monopoli
        //     false,      // milik sendiri
        //     "Player1"
        // );

        // tombol
        // s2->addButton("BUILD", "BUILD_HOUSE");
        // s2->addButton("MORTGAGE", "MORTGAGE_PROPERTY");
        // app.loadPopup(s2);

        // app.loadPopup(new MessagePopup(
        //     "Festival Tile",
        //     "Festival is here! Rent for all properties is doubled for 3 turns!",
        //     "src/views/Festival1.png"
        // ));

        while (!WindowShouldClose())
        {

            try
            {
                app.update();
                string command = app.getCommand();
                if (command != "NULL")
                {
                    cout << command << endl;
                    if (command == "START GAME")
                    {
                        app.enterGame();
                    }
                };
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
    }
    catch (const std::exception &e)
    {
        std::cerr << "FATAL INIT ERROR: " << e.what() << std::endl;
    }

    View2D::unloadFonts();
    CloseWindow();

    return 0;
}