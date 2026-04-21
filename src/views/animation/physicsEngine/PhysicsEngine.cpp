#include "../include/views/animation/physicsEngine/PhysicsEngine.hpp"

PhysicsEngine::PhysicsEngine(const float gravity, const float floorFriction, const float floorY, const float fps) : 
    gravityAcceleration(gravity), floorFriction(floorFriction), floorY(floorY), fps(fps) {}

PhysicsObject* PhysicsEngine::getObject(string objIdent) const {
    auto it = objects.find(objIdent);
        if (it != objects.end()) {
            return it->second;
        }
    return nullptr;
}

void PhysicsEngine::addObject(string objIdent, PhysicsObject& obj) {
    objects[objIdent] = &obj;
    obj.applyConstantForce(calculateGravitationalForce(objIdent), {0,0,0});
}

const float PhysicsEngine::getLowestDeltaTime() const {
    return 1.0f/fps;
}

Vector3 PhysicsEngine::calculateFrictionedForce(string objIdent, Vector3& force) {
    float mass = getObject(objIdent)->getMass();
    float newForceX = abs(force.x) - mass*gravityAcceleration*floorFriction;
    int forceXdir = force.x < 0 ? -1 : 1;
    if (newForceX < 0) {
        newForceX = 0;
    }
    float newForceZ = abs(force.z) - mass*gravityAcceleration*floorFriction;
    int forceZdir = force.z < 0 ? -1 : 1;
    if (newForceZ < 0) {
        newForceZ = 0;
    }
    return {forceXdir*newForceX, force.y, forceZdir*newForceZ};
}

Vector3 PhysicsEngine::calculateGravitationalForce(string objIdent) {
    return {0,-gravityAcceleration*getObject(objIdent)->getMass(), 0};
}

void printVector(const Vector3& v) {
    printf("(%.5f, %.5f, %.5f)\n", v.x, v.y, v.z);
}

void PhysicsEngine::handleObjectFloorCollision(string objIdent) {
    PhysicsObject& obj = *getObject(objIdent);
    
    Vector3 lastLinearVelocity = obj.getLinearVelocity();
    lastLinearVelocity.y *= -1;
    Vector3 collisionForce = (lastLinearVelocity*obj.getRestitution() - obj.getLinearVelocity())*obj.getMass()/getLowestDeltaTime();
    collisionForce = calculateFrictionedForce(objIdent, collisionForce);
    Vector3 collisionPoint = obj.getMinimumPoint();
    obj.setPosY(obj.getPos().y - collisionPoint.y);
    collisionPoint.y = 0;
    Vector3 relativeCollisionPoint = collisionPoint - obj.getPos();
    obj.applyInitialMomentum(collisionForce, relativeCollisionPoint, getLowestDeltaTime());
    obj.setLinearVelocity(lastLinearVelocity*obj.getRestitution());
    
    obj.doSurfaceRoll(calculateGravitationalForce(objIdent), collisionPoint, getLowestDeltaTime());

    if (abs(obj.getLinearVelocity().y) <= abs(gravityAcceleration*getLowestDeltaTime())) {
        Vector3 linVel = obj.getLinearVelocity();
        linVel.y = 0;
        obj.setLinearVelocity(linVel);
    }
    if (abs(obj.getAngularVelocity()) < 1) {
        obj.setAngularVelocity(0);
    }
    obj.setPos(obj.getPos() + obj.getLinearVelocity()*getLowestDeltaTime());
    obj.setObjectTouchingSurface(true);
}

bool PhysicsEngine::isObjectCollidingFloor(string objIdent) {
    return (getObject(objIdent)->getMinimumPoint().y - floorY) < 0.01;
}

bool PhysicsEngine::hasCubeObjectStopped(string objIdent) {
    PhysicsObject& obj = *getObject(objIdent);
    return (obj.getMinimumPoints().size() == 4 && obj.hasStopped());
}