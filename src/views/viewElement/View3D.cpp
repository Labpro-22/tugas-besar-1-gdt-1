#include "View3D.hpp"
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

void View3D::setPos(const Vector3& pos) {
    this->pos = pos;
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

void View3D::render() {
    model.transform = transformation;
    DrawModel(model, pos, 1, color);
    
}