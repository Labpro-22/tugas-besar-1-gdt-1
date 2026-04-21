#pragma once
#include "View3DCamera.hpp"
#include <functional>

class CameraMovement {
    private:
        View3DCamera& camera;
        float fps;
        bool moving;
        bool ended;
        bool interuptable;
        int frameProgress;
        function<void()> moveFunc;
        function<void()> moveEndFunc;
    public:
        CameraMovement(View3DCamera& camera, const float fps, bool interuptable, function<void()> moveFunc, function<void()> moveEndFunc);
        const bool isMoving() const;
        const bool hasEnded() const;
        const bool isInteruptable() const;
        void setMoveFunc(function<void()> animationFunc);
        void setMoveEndFunc(function<void()> animationEndFunc);
        void setLinearMoveAnimation(const Vector3& moveDest, const float duration);
        void setMoveToCameraAnimation(View3DCamera& destCamera, const float duration);
        void setRotationOnSelfAnimation(const float deg, const Vector3& axis, const float duration);
        void setRotationOnTargetAnimation(const float deg, const Vector3& axis, const float duration);
        void start();
        void pause();
        void end();
        void move();
};