#pragma once
#include "../../viewElement/View3D.hpp"
#include "View3DCamera.hpp"
#include "../ViewAnimation.hpp"
#include <functional>

class CameraMovement : public ViewAnimation{
    private:
        View3DCamera& camera;
    public:
        CameraMovement(View3DCamera& camera, const float fps, bool interuptable, function<void()> animationFunc, function<void()> animationEndFunc);
        void setLinearMoveAnimation(const Vector3& moveDest, const float duration);
        void setMoveSelfAnimation(const Vector3& moveDest, const float duration);
        void setMoveToCameraAnimation(View3DCamera& destCamera, const float duration);
        void setRotationOnSelfAnimation(const float deg, const Vector3& axis, const float duration);
        void setRotationOnTargetAnimation(const float deg, const Vector3& axis, const float duration);
        
};