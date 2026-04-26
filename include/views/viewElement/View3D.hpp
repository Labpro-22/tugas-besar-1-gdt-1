#pragma once
#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <map>
#include <vector>
using namespace std;

class View3DAnimation;

class View3D {
    protected:
        Vector3 pos;
        Model model;
        Matrix transformation;
        bool visible;
        Color color;
        Vector3 dimension;
        map<string, View3DAnimation*> animations;
    public:
        View3D();
        View3D(const Vector3& pos, const Model& model, const Color& color);
        virtual ~View3D();
        const Vector3 getPos() const;
        Model getModel() const;
        Matrix getTransformation() const;
        const BoundingBox getHitbox() const;
        const bool isHovered(Camera3D& cam) const;
        void movePosition(const Vector3& pos);
        void movePositionDelta(const Vector3& deltaPos);
        void setVisible(bool visible);
        void setPosX(float x);
        void setPosY(float y);
        void setPosZ(float z);
        void setTransform(const Matrix& m);
        void transform(const Matrix& m);
        void addAnimation(string animKey, View3DAnimation* anim);
        const bool isAnimationActive() const;
        View3DAnimation* getAnimation(string animKey) const;
        void animationCheck();
        virtual void render();
};

Mesh CopyMesh(Mesh original);