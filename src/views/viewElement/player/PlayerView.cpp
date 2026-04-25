#include "../include/views/viewElement/player/PlayerView.hpp"
#include "../include/views/viewElement/board/TileView.hpp"
#include "../include/views/viewElement/board/BoardView.hpp"
#include "../include/views/animation/ViewAnimation.hpp"
#include "../include/views/animation/camera/CameraMovement.hpp"

Model* PlayerView::playerModel = nullptr;

PlayerView::PlayerView(Player& player, BoardView* board, Color color, CameraManager* camManager) : player(player), board(board), currentTile(board->getGoTile()),
    View3D(board->getGoTile()->getPos(), *playerModel, color) {
    transform(MatrixRotate({1,0,0}, -M_PI/2));
    BoundingBox modelBB = GetModelBoundingBox(model);
    float scale = (TileView::getTileDim().y/4)/(modelBB.max.x - modelBB.min.x);
    transform(MatrixScale(scale, scale, scale));
    currentTile->handlePlayerEnteringTile(this);
    Vector3 camLoc = pos + Vector3Transform((Vector3){0, 2.0f, 10.0f}, 
                                            MatrixRotate({0,1,0}, (-currentTile->getCardinality() - 1)*(M_PI/2)));
    Vector3 camTarget = pos + (Vector3){0, 1.0f, 0};
    camManager->addCamera("CAM" + player.getUsername(), View3DCamera(camLoc, camTarget, 45));
    playerCam = &camManager->getCamera("CAM" + player.getUsername());
    playerCam->addMovement("FOLLOW_PLAYER", new CameraMovement(*playerCam, 120, true, [this](){
        playerCam->movePosition(this->pos + Vector3Transform((Vector3){0, 2.0f, 10.0f}, 
                                        MatrixRotate({0,1,0}, (-currentTile->getCardinality() - 1)*(M_PI/2))));
        playerCam->moveTargetPos(this->pos + (Vector3){0, 1.0f, 0});
    }, [](){}));
}

PlayerView::~PlayerView()
{
}

void PlayerView::unloadGlobalPlayerModel()
{
    if (playerModel != nullptr)
    {
        UnloadModel(*playerModel);
        delete playerModel;
        playerModel = nullptr;
    }
}

Player &PlayerView::getPlayer() { return player; }

string PlayerView::getPlayerCamKey() {
    return "CAM" + player.getUsername();
}

View3DCamera* PlayerView::getPlayerCamera() {
    return playerCam;
}

void PlayerView::moveToTile(TileView& destTile) {
    float originalY = getPos().y;
    View3DAnimation* moveStartAnim = new View3DAnimation(*this, 120, true, [](){}, [this, destTile, originalY](){
        function<void()>* recursive = new function<void()>;
        *recursive = [this, destTile, recursive, originalY](){
            if (currentTile->getTile()->getIndex() != destTile.getTile()->getIndex()) {
                View3DAnimation* moveAnim = new View3DAnimation(*this, 120, true, [](){}, *recursive);
                currentTile = board->getNextTile(*currentTile);
                moveAnim->setMoveAnimation(currentTile->getPassingPos() + (Vector3){0, getPos().y, 0}, 0.2);
                addAnimation("MOVE" + to_string(currentTile->getTile()->getIndex()), moveAnim);
                if (currentTile->isCornerTile()) {
                    this->playerCam->getMovement("FOLLOW_PLAYER")->pause();
                    Vector3 moveDest = (Vector3){currentTile->getPassingPos().x, this->pos.y, currentTile->getPassingPos().z} + 
                                        Vector3Transform((Vector3){0, 2.0f, 10.0f}, 
                                        MatrixRotate({0,1,0}, (-currentTile->getCardinality() - 1)*(M_PI/2)));
                    CameraMovement* rotateCam = new CameraMovement(*this->playerCam, 120, true, [this](){}, [this](){
                        this->playerCam->getMovement("FOLLOW_PLAYER")->start();
                    });
                    rotateCam->setMoveSelfAnimation(moveDest, 0.2);
                    this->playerCam->addMovement("ROTATE_ON_CORNER", rotateCam);
                    rotateCam->start();
                }
                getAnimation("MOVE" + to_string(currentTile->getTile()->getIndex()))->start();
            } else {
                View3DAnimation* moveEndAnim = new View3DAnimation(*this, 120, true, [](){}, [this, recursive](){
                    delete recursive;
                });
                moveEndAnim->setMoveYAnimation(originalY, 0.2);
                addAnimation("MOVE_END", moveEndAnim);
                this->getAnimation("MOVE_END")->start();
                currentTile->handlePlayerEnteringTile(this);
            }
        };
        (*recursive)();
    });
    moveStartAnim->setMoveAnimation((Vector3){currentTile->getPos().x, getPos().y, currentTile->getPos().z} + (Vector3){0,1,0}, 0.2);
    addAnimation("MOVE_START", moveStartAnim);
    this->getAnimation("MOVE_START")->start();
}

void PlayerView::moveSpaces(int moveVal) {
    int moveDest = currentTile->getTile()->getIndex() + moveVal;
    moveToTile(*board->getTileFromIdx(moveDest));
}

void PlayerView::loadPlayerModel(string filepath) {
    playerModel = new Model (LoadModel(filepath.c_str()));
}