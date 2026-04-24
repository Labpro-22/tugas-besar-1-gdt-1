#include "../include/views/animation/camera/CameraMovement.hpp"

CameraMovement::CameraMovement(View3DCamera &camera, const float fps, bool interuptable, function<void()> animationFunc, function<void()> animationEndFunc) : camera(camera), ViewAnimation(fps, interuptable, animationFunc, animationEndFunc)
{
    playing = true;
}

void CameraMovement::setLinearMoveAnimation(const Vector3 &moveDest, const float duration)
{
    Vector3 startingPos = camera.getPos();
    Vector3 targetCameraDelta = camera.getTarget() - camera.getPos();
    animationFunc = [moveDest, duration, startingPos, targetCameraDelta, this]()
    {
        if (frameProgress / fps >= duration)
        {
            camera.movePosition(moveDest);
            camera.movePosition(moveDest + targetCameraDelta);
            ended = true;
        }
        else
        {
            Vector3 posNow = startingPos + (moveDest - startingPos) * ((frameProgress / fps) / duration);
            camera.movePosition(posNow);
            camera.moveTargetPos(posNow);
        }
    };
}

void CameraMovement::setMoveSelfAnimation(const Vector3 &moveDest, const float duration)
{
    Vector3 startingPos = camera.getPos();
    animationFunc = [moveDest, duration, startingPos, this]()
    {
        if (frameProgress / fps >= duration)
        {
            camera.movePosition(moveDest);
            ended = true;
        }
        else
        {
            Vector3 posNow = startingPos + (moveDest - startingPos) * ((frameProgress / fps) / duration);
            camera.movePosition(posNow);
        }
    };
}

void CameraMovement::setMoveToCameraAnimation(View3DCamera &destCamera, const float duration)
{
    Vector3 startingPos = camera.getPos();
    Vector3 startingTargetPos = camera.getTarget();
    Vector3 startingUp = camera.up;
    View3DCamera *destPtr = &destCamera;

    animationFunc = [destPtr, duration, startingPos, startingTargetPos, startingUp, this]()
    {
        if (frameProgress / fps >= duration)
        {
            camera.movePosition(destPtr->getPos());
            camera.moveTargetPos(destPtr->getTarget());
            ended = true;
        }
        else
        {
            camera.movePosition(
                startingPos + (destPtr->getPos() - startingPos) * ((frameProgress / fps) / duration));
            camera.moveTargetPos(
                startingTargetPos + (destPtr->getTarget() - startingTargetPos) * ((frameProgress / fps) / duration));
        }
    };
}

void CameraMovement::setRotationOnSelfAnimation(const float deg, const Vector3 &axis, const float duration)
{
    animationFunc = [deg, axis, duration, this]()
    {
        if (frameProgress / fps >= duration)
        {
            ended = true;
        }
        else
        {
            camera.rotateAroundSelf(deg * (1 / fps) / duration, axis);
        }
    };
}

void CameraMovement::setRotationOnTargetAnimation(const float deg, const Vector3 &axis, const float duration)
{
    animationFunc = [deg, axis, duration, this]()
    {
        if (frameProgress / fps >= duration)
        {
            ended = true;
        }
        else
        {
            camera.rotateAroundTarget(deg * (1 / fps) / duration, axis);
        }
    };
}
