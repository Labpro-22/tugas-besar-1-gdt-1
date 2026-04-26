#include "../include/views/viewElement/player/DiceView.hpp"
#include "../include/views/animation/ViewAnimation.hpp"
#include <cstring>



Model createDiceModel(float diceEdges) {
    Mesh cubeMesh = GenMeshCube(diceEdges, diceEdges, diceEdges);
    int i = 0;
    vector<float> Xloc = {0.125, 0.375, 0.625};
    vector<float> Yloc = {0.25, 0.75};
    for (float y : Yloc) {
        for (float x : Xloc) {
            cubeMesh.texcoords[i*8] = x - 0.125;
            cubeMesh.texcoords[i*8 + 1] = y - 0.25;

            cubeMesh.texcoords[i*8 + 2] = x + 0.125;
            cubeMesh.texcoords[i*8 + 3] = y - 0.25;

            cubeMesh.texcoords[i*8 + 4] = x + 0.125;
            cubeMesh.texcoords[i*8 + 5] = y + 0.25;

            cubeMesh.texcoords[i*8 + 6] = x - 0.125;
            cubeMesh.texcoords[i*8 + 7] = y + 0.25;
            i++;
        }
    }
    Model cubeModel = LoadModelFromMesh(CopyMesh(cubeMesh));
    UnloadMesh(cubeMesh);

    Texture2D texture = LoadTexture("data/GUIAssets/dicefaces.png");
    cubeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    return cubeModel;
}

map<pair<int, int> , pair<Vector3, Vector3>> DiceView::rollMap = {
    {{1,1}, {{ 1.9f, -1, -0.67f }, { 0, -1.0f, -1.0f }}},
    {{1,2}, {{ 1.8f, -3, -0.5f }, { 0, 1.0f, -1.0f }}},
    {{1,3}, {{ 1.8f, -2, -0.5f }, { 0, -1.0f, 1.0f }}},
    {{1,4}, {{ 1.6f, -1, -0.56f }, { 0, -1.0f, 1.0f }}},
    {{1,5}, {{ 1.8f, -0.6, -0.77f }, { 0, -1.0f, -1.0f }}},
    {{1,6}, {{ 3.0f, -1, -0.64f }, { 0, -1.0f, -1.0f }}},

    {{2,1}, {{ 2.8f, -0.8, 0.7f }, { 0, 1.0f, -1.0f }}},
    {{2,2}, {{ 2.2f, -0.7, 0.55f }, { 0, -1.0f, 1.0f }}},
    {{2,3}, {{ 2.7f, -0.4, 0.85f }, { 0, -1.0f, 1.0f }}},
    {{2,4}, {{ 2.5f, -0.7, 0.9f }, { 0, 1.0f, -1.0f }}},
    {{2,5}, {{ 2.2f, -0.7, 0.55f }, { 0, -1.0f, -1.0f }}},
    {{2,6}, {{ 2.5f, -0.7, 1.0f } , { 0, -1.0f, -1.0f }}},
};

DiceView::DiceView(int dice1, int dice2, PlayerView* player, View3DCamera* diceCam) : originalPos(diceCam->getPos() + (Vector3){7.5f, -10.0f, 0}), 
    playing(false), diceCam(diceCam), done(false), diceNum({dice1,dice2}), player(player),
    throwButton(Interactable((Vector2){(float)GetScreenWidth()*0.4f, (float)GetScreenHeight()*0.4f}, false, false, "DISPLAY THROW", [](){}, [](){})),
    PhysicsEngine(9.80655, 0.5, 0, 120.0f) {
    float cubeEdges = 2.0f;
    Interactable* buttonPtr = &throwButton;
    Vector3 preOriginalPos = originalPos + (Vector3){-5.0f, -3.0f, 0};
    PhysicsObject* diceObject1 = new PhysicsObject(View3D(preOriginalPos + (Vector3){0, 0, -cubeEdges*3/4}, createDiceModel(cubeEdges), WHITE), 
                                          0.05f, 0.65f, pow(cubeEdges,2)/6);
    View3DAnimation* anim1 = new View3DAnimation(*diceObject1, 120, true, [](){}, [](){});
    View3DAnimation* animMove1 = new View3DAnimation(*diceObject1, 120, true, [](){}, [buttonPtr](){
        buttonPtr->setActive(true);
    });
    animMove1->setMoveAnimation(originalPos + (Vector3){0, 0, -cubeEdges*3/4}, 0.1);
    anim1->setWait(0.1, [animMove1](){ animMove1->start(); });
    anim1->start();
    diceObject1->addAnimation("WAIT_FOR_MOVE", anim1);
    diceObject1->addAnimation("MOVE_INITIAL", animMove1);
    addObject("DICE_1", diceObject1);
    PhysicsObject* diceObject2 = new PhysicsObject(View3D(preOriginalPos + (Vector3){0, 0, cubeEdges*3/4}, createDiceModel(cubeEdges), WHITE), 
                                          0.05f, 0.65f, pow(cubeEdges,2)/6);
    diceObject2->transform(MatrixRotate({0,0,1}, M_PI/2));
    View3DAnimation* anim2 = new View3DAnimation(*diceObject2, 120, true, [](){}, [](){});
    View3DAnimation* animMove2 = new View3DAnimation(*diceObject2, 120, true, [](){}, [buttonPtr](){
        buttonPtr->setActive(true);
    });
    animMove2->setMoveAnimation(originalPos + (Vector3){0, 0, cubeEdges*3/4}, 0.1);
    anim2->setWait(0.1, [animMove2](){ animMove2->start(); });
    anim2->start();
    diceObject2->addAnimation("WAIT_FOR_MOVE", anim2);
    diceObject2->addAnimation("MOVE_INITIAL", animMove2);
    addObject("DICE_2", diceObject2);

    throwButton.movePosition(GetWorldToScreen(originalPos, *diceCam));
}

DiceView::~DiceView() {
    for (auto pair : objects) {
        UnloadModel(pair.second->getModel());
        delete pair.second;
    }
}

Interactable* DiceView::getThrowButton() {
    return &throwButton;
}

bool DiceView::isDone() {
    return done;
}

int DiceView::getMoveValue() {
    return diceNum.first + diceNum.second;
}

PlayerView* DiceView::getPlayer() {
    return player;
}

void DiceView::initializeThrowDice() {
    pair<Vector3, Vector3> dice1Val = rollMap.at({1, diceNum.first});
    pair<Vector3, Vector3> dice2Val = rollMap.at({2, diceNum.second});
    getObject("DICE_1")->applyInitialMomentum(dice1Val.first , dice1Val.second, 0.2);
    getObject("DICE_2")->applyInitialMomentum(dice2Val.first , dice2Val.second, 0.2);
    playing = true;
}

void printVector3(const Vector3& v) {
    printf("(%.5f, %.5f, %.5f)\n", v.x, v.y, v.z);
}

void DiceView::showDiceOnCam() {
    View3DAnimation* moveAnim1 = new View3DAnimation(*getObject("DICE_1"), 120, false, [](){}, [](){});
    moveAnim1->setMoveAnimation(originalPos + (Vector3){2.0f, 3.0f, -2}, 0.4);
    moveAnim1->start();
    View3DAnimation* moveAnimRotate1 = new View3DAnimation(*getObject("DICE_1"), 120, false, [](){}, [](){});
    moveAnimRotate1->setRotateAnimation(60, Vector3Transform({0,1,1}, MatrixInvert(getObject("DICE_1")->getTransformation())), 0.4);
    moveAnimRotate1->start();
    getObject("DICE_1")->addAnimation("MOVE_TO_CAM", moveAnim1);
    getObject("DICE_1")->addAnimation("ROTATE_TO_CAM", moveAnimRotate1);
    View3DAnimation* moveAnim2 = new View3DAnimation(*getObject("DICE_2"), 120, false, [](){}, [this](){
        View3DAnimation* waitAnim = new View3DAnimation(*getObject("DICE_2"), 120, false, [](){}, [](){});\
        waitAnim->setWait(1, [this](){
            throwButton.setGameCommand("DISPLAY THROW_DONE");
            throwButton.sendCommand();
        });
        waitAnim->start();
        getObject("DICE_2")->addAnimation("WAIT_SEND", waitAnim);
    });
    moveAnim2->setMoveAnimation(originalPos + (Vector3){2.0f, 3.0f, 2}, 0.4);
    moveAnim2->start();
    View3DAnimation* moveAnimRotate2 = new View3DAnimation(*getObject("DICE_2"), 120, false, [](){}, [this](){
    });
    moveAnimRotate2->setRotateAnimation(60, Vector3Transform({0,1,1}, MatrixInvert(getObject("DICE_2")->getTransformation())), 0.4);
    moveAnimRotate2->start();
    getObject("DICE_2")->addAnimation("MOVE_TO_CAM", moveAnim2);
    getObject("DICE_2")->addAnimation("ROTATE_TO_CAM", moveAnimRotate2);
}

void DiceView::moveDiceOffScreen() {
    View3DAnimation* moveAnim1 = new View3DAnimation(*getObject("DICE_1"), 120, false, [](){}, [](){});
    moveAnim1->setMoveAnimation(getObject("DICE_1")->getPos() + (Vector3){0,0,-10.0f}, 0.3);
    moveAnim1->start();
    getObject("DICE_1")->addAnimation("MOVE_OFFSCREEN", moveAnim1);

    View3DAnimation* moveAnim2 = new View3DAnimation(*getObject("DICE_2"), 120, false, [](){}, [this](){
        done = true;
    });
    moveAnim2->setMoveAnimation(getObject("DICE_2")->getPos() + (Vector3){0,0,10.0f}, 0.3);
    moveAnim2->start();
    getObject("DICE_2")->addAnimation("MOVE_OFFSCREEN", moveAnim2);
}



void DiceView::update() {
    throwButton.interactionCheck();
    bool stopped = true;
    if (playing) {
        for (auto pair : objects) {
            if (!hasCubeObjectStopped(pair.first)) {
                stopped = false;
                if (isObjectCollidingFloor(pair.first)) {
                    handleObjectFloorCollision(pair.first);
                } else {
                    getObject(pair.first)->setObjectTouchingSurface(false);
                    getObject(pair.first)->updateObject(getLowestDeltaTime());
                }
            }
        }
        if (stopped) {
            playing = false;
            showDiceOnCam();
        }
    }

    
}

void DiceView::render() {
    for (auto pair : objects) {
        if (pair.second != nullptr) {
            pair.second->render();
        }
    }
}

