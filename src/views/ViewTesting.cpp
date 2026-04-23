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
        // RailroadProperty* r = new RailroadProperty(
        //     "GBR",                          // code
        //     "Stasiun Gambir",               // name
        //     200,                            // purchasePrice
        //     100,                            // mortgageValue
        //     {{1, 25}, {2, 50}, {3, 100}, {4, 200}} // rentTable
        // );

        // RailroadPopup* rp = new RailroadPopup(r, true, 1);
        // rp->addButton("BUY", "BUY_PROPERTY");
        // app.loadPopup(rp);

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