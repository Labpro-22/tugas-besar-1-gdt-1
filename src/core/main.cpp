#include "core/GameEngine.hpp"
#include "views/GUI.hpp"
#include "../include/utils/data/GameConfig.hpp"
#include "../include/utils/data/ConfigLoader.hpp"
#include "exception/GameException.hpp"

int main()
{
    SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1200, 800, "Nimonspoli");
    //ToggleFullscreen();
    SetTargetFPS(120);
    PlayerView::loadPlayerModel("data/GUIAssets/playerpawn.obj");
    View2D::addFont("Kabel", "data/GUIAssets/kabel.ttf");
    View2D::addFont("Orbitron", "data/GUIAssets/Orbitron-VariableFont_wght.ttf");

    try
    {
        ConfigLoader *CL = new ConfigLoader("data/config/default");
        GameConfig GC = CL->loadGameConfig();
        Board *b = CL->buildBoard(GC.getProperties(), GC);

        GUI app(120, *b);
        GameEngine engine(&app);
        //app.loadMainMenu();
        Player player = Player("Big Man", 500);
        Player player2 = Player("Little Man", 500);
        Player player3 = Player("Medium Man", 500);
        Player player4 = Player("Nonexistent Man", 500);
        player.addCard(new TeleportCard());
        player.addCard(new TeleportCard());
        player.addCard(new TeleportCard());
        app.loadPlayer(player);
        app.loadPlayer(player2);
        app.loadPlayer(player3);
        app.loadPlayer(player4);
        CardDeck<Card> chancePile;
        CardDeck<Card> comChestPile;
        chancePile.addCard(new ChanceCard(ChanceType::GO_TO_JAIL));
        chancePile.addCard(new ChanceCard(ChanceType::GO_TO_NEAREST_STATION));
        chancePile.addCard(new ChanceCard(ChanceType::MOVE_BACK_3));
        chancePile.addCard(new ChanceCard(ChanceType::GO_TO_JAIL));
        comChestPile.addCard(new CommunityChestCard(CommunityType::CAMPAIGN_FEE));
        comChestPile.addCard(new CommunityChestCard(CommunityType::CAMPAIGN_FEE));
        comChestPile.addCard(new CommunityChestCard(CommunityType::CAMPAIGN_FEE));
        comChestPile.addCard(new CommunityChestCard(CommunityType::CAMPAIGN_FEE));
        app.loadCardPiles(chancePile, comChestPile);
        app.loadDebuggingEntry();

        while (!WindowShouldClose() && !app.shouldExit())
        {
            if (IsKeyPressed(KEY_F11))
            {
                ToggleFullscreen();
            }

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