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

        // Test Popup untuk Property(sini)
        // PropertyPopup* popup = new PropertyPopup(
        //     "Medan",
        //     PropertyPopup::Type::STREET,
        //     PropertyPopup::Status::OWNED,
        //     200,
        //     100,
        //     false,           // isOtherPlayer
        //     "Player2"
        // );

        // // isi data street
        // popup->setStreetData(
        //     "YELLOW",
        //     {20, 40, 60, 100, 150, 200},
        //     20,     // base rent
        //     50,     // build cost
        //     2,      // level (2 rumah)
        //     true    // monopoli
        // );

        // // tombol (karena milik orang lain --> bayar)
        // popup->addButton("PAY RENT", "PAY_RENT");
        // popup->addButton("SELL 1 HOUSE", "SELL_BUILDING"); // (tambahin tombol lain juga boleh)

        app.loadPopup(popup);

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