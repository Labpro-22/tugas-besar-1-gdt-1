#pragma once
#include "../viewElement/View2D.hpp"
#include "../viewElement/View3D.hpp"


class ViewAnimation {
    protected:
        float fps;
        bool playing;
        bool ended;
        bool interuptable;
        int frameProgress;
        function<void()> animationFunc;
        function<void()> animationEndFunc;
    public:
        ViewAnimation(float fps, bool interuptable, function<void()> animationFunc, function<void()> animationEndFunc);
        const bool isPlaying() const;
        const bool hasEnded() const;
        const bool isInteruptable() const;
        void setAnimationFunc(function<void()> animationFunc);
        void setAnimationEndFunc(function<void()> animationEndFunc);
        void start();
        void pause();
        void end();
        void play();
};

class View2DAnimation : public ViewAnimation {
    private:
        View2D& view;
    public:
        View2DAnimation(View2D& view, const float fps, bool interuptable, function<void()> animationFunc, function<void()> animationEndFunc);
        void setMoveAnimation(const Vector2 moveDest, const float duration);
        void setScaleAnimation(const float newScale, const float duration);
        void setFadeAnimation(const float newOpacity, const float duration);
};

class View3DAnimation : public ViewAnimation {
    private:
        View3D& view;
    public:
        View3DAnimation(View3D& view, const float fps, bool interuptable, function<void()> animationFunc, function<void()> animationEndFunc);
        void setMoveAnimation(const Vector3 moveDest, const float duration);
        void setMoveYAnimation(float moveDest, const float duration);
};