#include "views/GUI.hpp"
#include "../include/utils/data/GameConfig.hpp"
#include "../include/utils/data/ConfigLoader.hpp"


int main() {    
    const int screenWidth = 1200;
    const int screenHeight = 800;
    SetTraceLogLevel(LOG_NONE);
    InitWindow(screenWidth, screenHeight, "Nimonspoli");
    View2D::addFont("Orbitron", "data/GUIAssets/Orbitron-VariableFont_wght.ttf");
    SetTargetFPS(120);
    
    ConfigLoader CL("data/default");
    GameConfig GC = CL.loadGameConfig();
    
    Board& b = *CL.buildBoard(GC.getProperties(), GC);
    GUI app(120, b);
    app.loadMainMenu();
    app.loadDebuggingEntry();
    while (!WindowShouldClose()) {
        ClearBackground(RAYWHITE);
        app.update();
        string command = app.getCommand();
        if (command != "NULL") { 
            cout<<command<<endl; 
            if (command == "START GAME") { app.enterGame(); }
        };
        BeginDrawing();
        app.display();
        DrawFPS(10,10);
        EndDrawing();
    }
    View2D::unloadFonts();
    CloseWindow();
   
    return 0;
}