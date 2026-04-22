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
    
    ConfigLoader CL("data/default");
    GameConfig GC = CL.loadGameConfig();
    
    Board& b = *CL.buildBoard(GC.getProperties(), GC);
    GUI app(120, b);
    app.loadMainMenu();
    Player player = Player("Big Man", 500);
    Player player2 = Player("Little Man", 500);
    Player player3 = Player("Medium Man", 500);
    Player player4 = Player("Nonexistent Man", 500);
    app.loadPlayer(player);
    app.loadPlayer(player2);
    app.loadPlayer(player3);
    app.loadPlayer(player4);
    app.loadDebuggingEntry();
    while (!WindowShouldClose()) {
        ClearBackground(RAYWHITE);
        try {
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
            catch (const GameException &e) {
                app.loadPopup(new ExceptionPopup(e.getErrorCode(), e.what()));
            }
            catch (const std::exception &e) {
                app.loadPopup(new ExceptionPopup(500, e.what()));
            }catch (const std::exception &e) {
                std::cerr << "FATAL INIT ERROR: " << e.what() << std::endl;
            }
        BeginDrawing();
        app.display();
        DrawFPS(10,10);
        EndDrawing();
    }
    

    View2D::unloadFonts();
    CloseWindow();

    return 0;
}