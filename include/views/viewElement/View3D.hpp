#pragma once
#include "raylib.h"
#include "raymath.h"
#include <iostream>

class View3D {
    protected:
        Vector3 pos;
        Model model;
        Matrix transformation;
        Color color;
        Vector3 dimension;
    public:
        View3D();
        View3D(const Vector3& pos, const Model& model, const Color& color);
        const Vector3 getPos() const;
        void setPos(const Vector3& pos);
        void setPosX(float x);
        void setPosY(float y);
        void setPosZ(float z);
        void setTransform(Matrix& m);
        void transform(Matrix& m);
        virtual void render();
};