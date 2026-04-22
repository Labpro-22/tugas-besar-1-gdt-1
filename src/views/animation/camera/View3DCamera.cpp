#include "../include/views/animation/camera/View3DCamera.hpp"
#include "../include/views/animation/camera/CameraMovement.hpp"
#include <set>

View3DCamera::View3DCamera() : Camera3D({0}) {}

View3DCamera::View3DCamera(const Vector3& pos, const Vector3& target, const float fovy) : Camera3D({0}), closed(false){
    position = pos;
    this->target = target;
    up = (Vector3){ 0.0f, 1.0f, 0.0f };
    this->fovy = fovy;
    projection = CAMERA_PERSPECTIVE;
}

View3DCamera::~View3DCamera() {
    //for(auto move : movements) { delete move.second; }
}

const Vector3 View3DCamera::getPos() const { return position; }
const Vector3 View3DCamera::getTarget() const { return target; }
const Vector3 View3DCamera::getUp() const { return up; }
const float View3DCamera::getFOV() const { return fovy; }
const bool View3DCamera::isClosed() const { return closed; }

void View3DCamera::movePosition(const Vector3& pos) { position = pos; }
void View3DCamera::movePositionDelta(const Vector3& deltaPos) { position += deltaPos; }
void View3DCamera::moveTargetPos(const Vector3& pos) { target = pos; }
void View3DCamera::moveTargetPosDelta(const Vector3& deltaPos) { target += deltaPos; }
void View3DCamera::setUp(const Vector3& pos) { up = pos; }
void View3DCamera::setUpDelta(const Vector3& deltaUp) { up += deltaUp; }

void View3DCamera::rotateAroundPoint(const float deg, const Vector3& axis, const Vector3& point) {
    float radian = deg * (M_PI/180.0f);
    Matrix pivotEnd = MatrixTranslate(point.x, point.y, point.z);
    Matrix pivotStart = MatrixTranslate(-point.x, -point.y, -point.z);
    Matrix rotation = MatrixRotate(axis, radian);
    position = Vector3Transform(position, pivotEnd*rotation*pivotStart);
    target = Vector3Transform(target, pivotEnd*rotation*pivotStart);
}

void View3DCamera::rotateAroundSelf(const float deg, const Vector3& axis) {
    return rotateAroundPoint(deg, axis, position);
}

void View3DCamera::rotateAroundTarget(const float deg, const Vector3& axis) {
    return rotateAroundPoint(deg, axis, target);
}

void View3DCamera::addMovement(string moveKey, CameraMovement* movement) { movements[moveKey] = movement; }
CameraMovement* View3DCamera::getMovement(string moveKey) { return movements.at(moveKey); }

void View3DCamera::pauseAllMovement() {
    for (auto pair : movements) {
        pair.second->pause();
    }
}

void View3DCamera::resumeAllMovement() {
    for (auto pair : movements) {
        pair.second->start();
    }
}


void View3DCamera::updateMovement() {
    vector<string> doneMovements;
    for(auto move : movements) {
        move.second->play();
        if (move.second->hasEnded() || closed) {
            doneMovements.push_back(move.first);
        }
    }
    
    for (string moveKey : doneMovements) {
        delete movements[moveKey];
        movements.erase(moveKey);
    }
}

void View3DCamera::close() { closed = true; }


Camera3D& View3DCamera::mount() {
    updateMovement();
    UpdateCamera(this, 0);
    return *this;
}