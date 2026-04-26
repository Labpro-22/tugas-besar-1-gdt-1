#pragma once
#include "../View3D.hpp"
#include "../../animation/camera/View3DCamera.hpp"
#include "../../animation/camera/CameraManager.hpp"
#include "../../../models/Player/Player.hpp"

class TileView;

class BoardView;

class PlayerView : public View3D
{
private:
    Player &player;
    TileView *currentTile;
    BoardView *board;
    View3DCamera *playerCam;
    static Model *playerModel;

public:
    PlayerView(Player &player, BoardView *board, Color color, CameraManager *camManager);
    ~PlayerView();

    Player &getPlayer();
    string getPlayerCamKey();
    View3DCamera *getPlayerCamera();
    void setCurrentTile(TileView *currentTile);
    void moveSpaces(int moveVal);
    void moveToTile(TileView& tile, bool forward);
    void teleportToTile(TileView& destTile);
    void sendPlayerToJail();
    static void loadPlayerModel(string filePath);
    static void unloadGlobalPlayerModel();
};