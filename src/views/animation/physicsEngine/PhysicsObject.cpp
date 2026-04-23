#include "../include/views/animation/physicsEngine/PhysicsObject.hpp"


PhysicsObject::PhysicsObject() : 
    View3D(), mass(0), restitution(0), inertia(0), linearVelocity({0,0,0}), linearAcceleration({0,0,0}), rotationAxis({0,0,0}), 
    angularVelocity(0), angularAcceleration(0), surfaceRollAngularVelocity(0), objectTouchingSurface(false) {
        boundingBox = (BoundingBox) {pos - dimension/2, 
                                     pos + dimension/2};
        transformedBB = (BoundingBox) {pos - dimension/2, 
                                     pos + dimension/2};
    }

PhysicsObject::PhysicsObject(const View3D& object3D, float mass, float res, float inertiaConstant) :
    View3D(object3D), mass(mass), restitution(res), inertia(inertiaConstant*mass),
    linearVelocity({0,0,0}), linearAcceleration({0,0,0}), rotationAxis({0,0,0}), 
    angularVelocity(0), angularAcceleration(0), surfaceRollAngularVelocity(0), objectTouchingSurface(false) {
        boundingBox = (BoundingBox) {pos - dimension/2, 
                                     pos + dimension/2};
        transformedBB = (BoundingBox) {pos - dimension/2, 
                                     pos + dimension/2};
    }




const float PhysicsObject::getMass() const {
    return mass;
}

const float PhysicsObject::getRestitution() const {
    return restitution;
}

const float PhysicsObject::getInertia() const {
    return inertia;
}

const BoundingBox PhysicsObject::getBoundingBox() {
    updateBoundingBox();
    return transformedBB;
}

const Vector3 PhysicsObject::getLinearVelocity() const {
    return linearVelocity;
}

const Vector3 PhysicsObject::getLinearAcceleration() const {
    return linearAcceleration;
}

const float PhysicsObject::getAngularVelocity() const {
    return angularVelocity;
}

const float PhysicsObject::getAngularAcceleration() const {
    return angularAcceleration;
}

const Vector3 PhysicsObject::getRotationAxis() const {
    return rotationAxis;
}

const Vector3 PhysicsObject::getSurfaceRollAxis() const {
    return surfaceRollAxis;
}

const float PhysicsObject::getSurfaceRollAngularVelocity() const {
    return surfaceRollAngularVelocity;
}

const vector<Vector3> PhysicsObject::getMinimumPoints() {
    Vector3 pivot = {(boundingBox.min.x + boundingBox.max.x)/2, 
                     (boundingBox.min.y + boundingBox.max.y)/2, 
                     (boundingBox.min.z + boundingBox.max.z)/2};
    Matrix rotationMatrix = transformation;
    std::vector<Vector3> bbCorners = {
        {boundingBox.min.x, boundingBox.min.y, boundingBox.min.z},
        {boundingBox.min.x, boundingBox.min.y, boundingBox.max.z},
        {boundingBox.min.x, boundingBox.max.y, boundingBox.min.z},
        {boundingBox.min.x, boundingBox.max.y, boundingBox.max.z},
        {boundingBox.max.x, boundingBox.min.y, boundingBox.min.z},
        {boundingBox.max.x, boundingBox.min.y, boundingBox.max.z},
        {boundingBox.max.x, boundingBox.max.y, boundingBox.min.z},
        {boundingBox.max.x, boundingBox.max.y, boundingBox.max.z}
    };
    vector<Vector3> rotatedBBCorners;
    float minY = std::numeric_limits<float>::max();
    for (Vector3 corner : bbCorners) {
        Vector3 transformedVector = Vector3Add(Vector3Transform(Vector3Subtract(corner, pivot), rotationMatrix), pivot);
        minY = std::min<float>(minY, transformedVector.y);
        rotatedBBCorners.push_back(transformedVector);
    }
    vector<Vector3> minPoints;
    for (Vector3& corner : rotatedBBCorners) {
        if (abs(corner.y -  minY) <= 0.05) {
            minPoints.push_back(corner);
        }
    }
    
    return minPoints;
}

const Vector3 PhysicsObject::getMinimumPoint() {
    updateBoundingBox();
    float totalX = 0;
    float totalY = 0;
    float totalZ = 0;
    int n = 0;
    for (Vector3 corner: getMinimumPoints()) {
            totalX += corner.x;
            totalY += corner.y;
            totalZ += corner.z;
            n++;
    }
    return {totalX/n, totalY/n, totalZ/n};
}

void PhysicsObject::updateBoundingBox() {
    boundingBox = (BoundingBox) {pos - dimension/2, 
                                pos + dimension/2};
    std::vector<Vector3> bbCorners = {
        {boundingBox.min.x, boundingBox.min.y, boundingBox.min.z},
        {boundingBox.min.x, boundingBox.min.y, boundingBox.max.z},
        {boundingBox.min.x, boundingBox.max.y, boundingBox.min.z},
        {boundingBox.min.x, boundingBox.max.y, boundingBox.max.z},
        {boundingBox.max.x, boundingBox.min.y, boundingBox.min.z},
        {boundingBox.max.x, boundingBox.min.y, boundingBox.max.z},
        {boundingBox.max.x, boundingBox.max.y, boundingBox.min.z},
        {boundingBox.max.x, boundingBox.max.y, boundingBox.max.z}
    };
    std::vector<Vector3> bbCornersRotated;
    float minX = std::numeric_limits<float>::max(), minY = std::numeric_limits<float>::max(), minZ = std::numeric_limits<float>::max();
    float maxX = -minX, maxY = -minY, maxZ = -minZ;
    for (Vector3 corner : bbCorners) {
        Vector3 transformedVector = Vector3Transform(corner - pos, transformation) + pos;
        minX = std::min<float>(minX, transformedVector.x);
        minY = std::min<float>(minY, transformedVector.y);
        minZ = std::min<float>(minZ, transformedVector.z);
        maxX = std::max<float>(maxX, transformedVector.x);
        maxY = std::max<float>(maxY, transformedVector.y);
        maxZ = std::max<float>(maxZ, transformedVector.z);
    }
    transformedBB = (BoundingBox) {{minX, minY, minZ}, {maxX, maxY, maxZ}};
}

void PhysicsObject::updateObject(float deltaTime) {
    linearVelocity += linearAcceleration*deltaTime;
    angularVelocity += angularAcceleration*deltaTime;
    pos += linearVelocity*deltaTime;
    transformation = MatrixRotate(rotationAxis, angularVelocity*deltaTime)*transformation;
    updateBoundingBox();
}

void PhysicsObject::setLinearVelocity(const Vector3& linVel) {
    linearVelocity = linVel;
}
void PhysicsObject::setLinearAcceleration(const Vector3& linAcc) {
    linearAcceleration = linAcc;
}
void PhysicsObject::setAngularVelocity(const float angVel) {
    angularVelocity = angVel;
}
void PhysicsObject::setAngularAcceleration(const float angAcc) {
    angularAcceleration = angAcc;
}
void PhysicsObject::setRotationAxis(const Vector3& axis) {
    rotationAxis = axis;
}
void PhysicsObject::setSurfaceRollAxis(const Vector3& axis) {
    surfaceRollAxis = axis;
}
void PhysicsObject::setSurfaceRollAngularVelocity(const float sfAngVel) {
    surfaceRollAngularVelocity = sfAngVel;
}
void PhysicsObject::setObjectTouchingSurface(bool t) {
    objectTouchingSurface = t;
}


void PhysicsObject::applyInitialMomentum(const Vector3& force, const Vector3& appliedAt, float deltaTime) {
    Vector3 torque = Vector3CrossProduct(force, appliedAt);
    rotationAxis = Vector3Normalize(torque);
    angularVelocity = (Vector3Length(torque)*deltaTime)/inertia;
    linearVelocity = force*deltaTime/mass + linearVelocity;
}

void PhysicsObject::doSurfaceRoll(const Vector3& force, const Vector3& rollPoint, float deltaTime) {
    Vector3 armLengthCOM = pos - rollPoint;
    Vector3 torque = Vector3CrossProduct(armLengthCOM, force);
    if (!objectTouchingSurface) {
        surfaceRollAngularVelocity = 0;
    }
    surfaceRollAxis = Vector3Normalize(torque);
    surfaceRollAngularVelocity += (Vector3Length(torque)*deltaTime)/inertia;
    Matrix rollTransform = MatrixTranslate(armLengthCOM.x, armLengthCOM.y, armLengthCOM.z);
    rollTransform = MatrixRotateXYZ(surfaceRollAxis*surfaceRollAngularVelocity*deltaTime)*rollTransform;
    rollTransform = MatrixTranslate(-armLengthCOM.x, -armLengthCOM.y, -armLengthCOM.z)*rollTransform;
    transformation *= rollTransform;
    pos *= rollTransform;
    pos.y -= getMinimumPoint().y;
}

void PhysicsObject::applyConstantForce(const Vector3& force, const Vector3& appliedAt) {
    linearAcceleration = force/mass;
}

const bool PhysicsObject::hasStopped() const {
    return Vector3Equals(linearVelocity, {0,0,0}) && angularVelocity == 0;
}
