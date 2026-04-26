#pragma once
#include "../../animation/camera/View3DCamera.hpp"
#include "../../animation/physicsEngine/PhysicsEngine.hpp"
#include "../Interactable.hpp"
#include "PlayerView.hpp"

#include <map>


class DiceView : public PhysicsEngine {
    private :
        bool playing;
        PlayerView* player;
        pair<int, int> diceNum;
        Vector3 originalPos;
        Interactable throwButton;
        View3DCamera* diceCam;
        bool done;
        static map<pair<int,int>, pair<Vector3, Vector3>> rollMap;
    public :
        DiceView(int dice1, int dice2, PlayerView* player, View3DCamera* diceCam);
        ~DiceView();
        Interactable* getThrowButton();
        int getMoveValue();
        PlayerView* getPlayer();
        bool isDone();
        void initializeThrowDice();
        void moveDiceOffScreen();
        void showDiceOnCam();
        void update();
        void render();
};