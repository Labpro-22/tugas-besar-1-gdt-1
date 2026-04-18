#pragma once
#include "PhysicsObject.hpp"
#include <map>


class PhysicsEngine {
    private :
        map<string, PhysicsObject*> objects;
        const float gravityAcceleration;
        const float floorFriction;
        const int fps;
        const float floorY;
        PhysicsObject emptyObject;
    public :
        PhysicsEngine(const float gravity, const float floorFriction, const float floorY, const float fps);
        void addObject(string objIdent, PhysicsObject& obj);
        PhysicsObject* getObject(string objIdent) const;
        const float getLowestDeltaTime() const;
        void handleObjectFloorCollision(string objIdent);
        Vector3 calculateFrictionedForce(string objIdent, Vector3& force);
        Vector3 calculateGravitationalForce(string objIdent);
        
        bool isObjectCollidingFloor(string objIdent);
        bool hasCubeObjectStopped(string objIdent);
};


