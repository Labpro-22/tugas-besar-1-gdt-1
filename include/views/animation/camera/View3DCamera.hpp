#pragma once
#include "raylib.h"
#include "raymath.h"
#include <map>
#include <iostream>
using namespace std;

class CameraMovement;

class View3DCamera : public Camera3D {
    private :
        map<string, CameraMovement*> movements;
    public :
        View3DCamera();
        View3DCamera(const Vector3& pos, const Vector3& target, const float fovy);
        ~View3DCamera();
        const Vector3 getPos() const;
        const Vector3 getTarget() const;
        const Vector3 getUp() const;
        
        void movePosition(const Vector3& pos);
        void movePositionDelta(const Vector3& deltaPos);
        void moveTargetPos(const Vector3& pos);
        void moveTargetPosDelta(const Vector3& deltaPos);
        void setUp(const Vector3& up);
        void setUpDelta(const Vector3& deltaUp);
        void rotateAroundPoint(const float deg, const Vector3& axis, const Vector3& point);
        void rotateAroundSelf(const float deg, const Vector3& axis);
        void rotateAroundTarget(const float deg, const Vector3& axis);
        void addMovement(string moveKey, CameraMovement* movement);
        CameraMovement* getMovement(string moveKey);
        void updateMovement();
        Camera3D& mount();
};