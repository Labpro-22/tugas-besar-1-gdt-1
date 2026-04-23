#include "../include/views/viewElement/View3D.hpp"
#include "../include/views/animation/ViewAnimation.hpp"
#include <iostream>
using namespace std;

View3D::View3D() :
    pos({0,0}), model(LoadModelFromMesh(GenMeshCube(0,0,0))), transformation(MatrixIdentity()),
    color({0,0,0,0}), dimension({0,0,0}) {}

View3D::View3D(const Vector3& pos, const Model& model, const Color& color) :
    pos(pos), model(model), transformation(model.transform), color(color) {
        BoundingBox modelBB = GetModelBoundingBox(model);
        dimension = modelBB.max - modelBB.min;
    }



const Vector3 View3D::getPos() const {
    return pos;
}

void View3D::movePosition(const Vector3& pos) {
    this->pos = pos;
}

void View3D::movePositionDelta(const Vector3& deltaPos) {
    this->pos += deltaPos;
}


void View3D::setPosX(float x) {
    pos.x = x;
}
void View3D::setPosY(float y) {
    pos.y = y;
}
void View3D::setPosZ(float z) {
    pos.z = z;
}

void View3D::setTransform(const Matrix& m) {
    transformation = m;
}


void View3D::transform(const Matrix& m) {
    transformation = m*transformation;
}

void View3D::addAnimation(string animKey, View3DAnimation* anim) { animations[animKey] = anim; }
View3DAnimation* View3D::getAnimation(string animKey) const { return animations.at(animKey); }

void View3D::animationCheck() {
    vector<string> doneAnimations;
    for(auto anim : animations) {
        anim.second->play();
        if (anim.second->hasEnded()) {
            doneAnimations.push_back(anim.first);
        }
    }
    
    for (string animKey : doneAnimations) {
        delete animations[animKey];
        animations.erase(animKey);
    }
}

void View3D::render() {
    animationCheck();
    model.transform = transformation;
    DrawModel(model, pos, 1, color);
}