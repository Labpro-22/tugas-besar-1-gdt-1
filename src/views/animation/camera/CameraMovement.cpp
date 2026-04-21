#include "../include/views/animation/camera/CameraMovement.hpp"

CameraMovement::CameraMovement(View3DCamera& camera, const float fps, bool interuptable, function<void()> moveFunc, function<void()> moveEndFunc) :
    camera(camera), fps(fps), interuptable(interuptable), moveFunc(moveFunc), moveEndFunc(moveEndFunc), 
    moving(true), ended(false), frameProgress(0) {}

const bool CameraMovement::isMoving() const { return moving; }
const bool CameraMovement::hasEnded() const { return ended; }
const bool CameraMovement::isInteruptable() const { return interuptable; }


void CameraMovement::setMoveFunc(function<void()> moveFunc) { this->moveFunc = moveFunc; }
void CameraMovement::setMoveEndFunc(function<void()> moveEndFunc) { this->moveEndFunc = moveEndFunc; }

void CameraMovement::setLinearMoveAnimation(const Vector3& moveDest, const float duration) {
    Vector3 startingPos = camera.getPos();
    Vector3 targetCameraDelta = camera.getTarget() - camera.getPos();
    moveFunc = [moveDest, duration, startingPos, targetCameraDelta, this](){
        if (frameProgress/fps >= duration) {
            camera.movePosition(moveDest);
            camera.movePosition(moveDest + targetCameraDelta);
            ended = true;
        } else {
            Vector3 posNow = startingPos + (moveDest - startingPos)*((frameProgress/fps)/duration);
            camera.movePosition(posNow);
            camera.moveTargetPos(posNow);
        }
    };
}


void CameraMovement::setMoveToCameraAnimation(View3DCamera& destCamera, const float duration) {
    Vector3 startingPos = camera.getPos();
    Vector3 startingTargetPos = camera.getTarget();
    Vector3 startingUp = camera.up;
    moveFunc = [destCamera, duration, startingPos, startingTargetPos, startingUp, this](){
        if (frameProgress/fps >= duration) {
            camera.movePosition(destCamera.getPos());
            camera.moveTargetPos(destCamera.getTarget());
            ended = true;
        } else {
            camera.movePosition(startingPos + (destCamera.getPos() - startingPos)*((frameProgress/fps)/duration));
            camera.moveTargetPos(startingTargetPos + (destCamera.getTarget() - startingTargetPos)*((frameProgress/fps)/duration));
        }
    };
}

void CameraMovement::setRotationOnSelfAnimation(const float deg, const Vector3& axis, const float duration) {
    moveFunc = [deg, axis, duration, this](){
        if (frameProgress/fps >= duration) {
            ended = true;
        } else {
            camera.rotateAroundSelf(deg*(1/fps)/duration, axis);
        }
    };
}

void CameraMovement::setRotationOnTargetAnimation(const float deg, const Vector3& axis, const float duration) {
        moveFunc = [deg, axis, duration, this](){
        if (frameProgress/fps >= duration) {
            ended = true;
        } else {
            camera.rotateAroundTarget(deg*(1/fps)/duration, axis);
        }
    };
}

void CameraMovement::start() { moving = true; }
void CameraMovement::pause() { moving = false; }
void CameraMovement::end() { ended = true; }
void CameraMovement::move() {
    if (moving) { 
        moveFunc(); 
        frameProgress++;
    }
    if (ended) { moveEndFunc(); }
    
}
