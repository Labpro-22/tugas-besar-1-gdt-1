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
        ConfigLoader CL("data/config/default");
        GameConfig GC = CL.loadGameConfig();

        Board &b = *CL.buildBoard(GC.getProperties(), GC);
        GUI app(120, b);
        app.loadMainMenu();
        app.loadDebuggingEntry();

        // Test Popup untuk Property
        // app.loadPopup(new PropertyPopup(
        //     "Medan",                         // nama properti
        //     "STREET",                        // tipe properti ("STREET", "RAILROAD", atau "UTILITY")
        //     "BANK",                         // status kepemilikan ("BANK", "OWNED", atau "MORTGAGED")
        //     200,                             // harga beli
        //     100,                             // nilai gadai (mortgage value)

        //     2,                               // 2 rumah dibangun  
        //     false,                           // milik orang lain

        //     "Player2",                       // owner properti
        //     "YELLOW",                        // warna group (hanya untuk Street)
        //     {20, 40, 60, 100, 150, 200},     // rentTable untuk Street (index 0-5: 0 rumah - hotel)
        //     50                               // harga bangun (hanya untuk Street)
        // ));                                  // railroadRent dan utilityMultiplier DIKOSONGKAN karena bukan Railroad/Utility

        // app.loadPopup(new PropertyPopup(
        //     "Stasiun Gambir",                // nama properti
        //     "RAILROAD",                      // tipe properti ("STREET", "RAILROAD", atau "UTILITY")
        //     "BANK",                     // status kepemilikan ("BANK", "OWNED", atau "MORTGAGED")
        //     0,                               // tidak ada harga beli untuk Railroad (dianggap 0 karena tidak relevan)
        //     100,                             // nilai gadai (mortgage value)

        //     3,                               // punya 3 railroad
        //     true,                            // milik orang lain

        //     "Player1",                       // owner properti
        //     "",                              // warna group DIKOSONGKAN karena bukan Street
        //     {},                              // rentTable DIKOSONGKAN karena bukan Street
        //     0,                               // harga bangun DIKOSONGKAN karena bukan Street
        //     {25, 50, 100, 200}               // railroadRent berdasarkan jumlah kepemilikan Railroad yang sama (index 0-3: punya 1-4 railroad)
        // ));  

        // app.loadPopup(new PropertyPopup( 
        //     "PLN",                           // nama properti
        //     "UTILITY",                       // tipe properti ("STREET", "RAILROAD", atau "UTILITY")
        //     "OWNED",                         // status kepemilikan ("BANK", "OWNED", atau "MORTGAGED")
        //     0,                               // tidak ada harga beli untuk Utility (dianggap 0 karena tidak relevan)
        //     75,                              // nilai gadai (mortgage value)

        //     2,                               // punya 2 utility
        //     false,                           // milik orang lain

        //     "Player2",                       // owner properti
        //     "",                              // warna group DIKOSONGKAN karena bukan Street
        //     {},                              // rentTable DIKOSONGKAN karena bukan Street
        //     0,                               // harga bangun DIKOSONGKAN karena bukan Street
        //     {},                              // railroadRent DIKOSONGKAN karena bukan Railroad
        //     {4, 10}                          // utilityMultiplier berdasarkan jumlah kepemilikan Utility yang sama (index 0-1: punya 1-2 utility)
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