#pragma once
#include "../viewElement/View3D.hpp"
#include <math.h>
#include <vector>
using namespace std;


class PhysicsObject : public View3D {
    private :
        float mass;
        float restitution;
        float inertia;
        BoundingBox boundingBox;
        BoundingBox transformedBB;
        Vector3 linearVelocity;
        Vector3 linearAcceleration;
        Vector3 rotationAxis;
        float angularVelocity;
        float angularAcceleration;

        bool objectTouchingSurface;
        Vector3 surfaceRollAxis;
        float surfaceRollAngularVelocity;
    public :
        PhysicsObject();
        PhysicsObject(const View3D& object3D, float mass, float res, float inertiaConstant);
        const float getMass() const;
        const float getRestitution() const;
        const float getInertia() const;
        const BoundingBox getBoundingBox();
        const Vector3 getLinearVelocity() const;
        const Vector3 getLinearAcceleration() const;
        const float getAngularVelocity() const;
        const float getAngularAcceleration() const;
        const Vector3 getRotationAxis() const;
        const Vector3 getSurfaceRollAxis() const;
        const float getSurfaceRollAngularVelocity() const;
        const vector<Vector3> getMinimumPoints();
        const Vector3 getMinimumPoint();
        void updateBoundingBox();
        void updateObject(const float deltaTime);
        void setLinearVelocity(const Vector3& linVel);
        void setLinearAcceleration(const Vector3& linAcc);
        void setAngularVelocity(const float angVel);
        void setAngularAcceleration(const float angAcc);
        void setRotationAxis(const Vector3& axis);
        void setSurfaceRollAxis(const Vector3& axis);
        void setSurfaceRollAngularVelocity(const float sfAngVel);
        void setObjectTouchingSurface(bool t);

        void applyConstantForce(const Vector3& force, const Vector3& appliedAt);
        void applyInitialMomentum(const Vector3& force, const Vector3& appliedAt, float deltaTime);
        void doSurfaceRoll(const Vector3& force, const Vector3& rollPoint, float deltaTime);

        const bool hasStopped() const;
};