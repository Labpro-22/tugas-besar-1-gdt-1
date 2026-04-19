#pragma once
#include "../viewElement/View2D.hpp"


class ViewAnimation {
    private:
        View2D& view;
        float fps;
        bool playing;
        bool ended;
        bool interuptable;
        int frameProgress;
        function<void()> animationFunc;
        function<void()> animationEndFunc;
    public:
        ViewAnimation(View2D& view, const float fps, bool interuptable, function<void()> animationFunc, function<void()> animationEndFunc);
        const bool isPlaying() const;
        const bool hasEnded() const;
        const bool isInteruptable() const;
        void setAnimationFunc(function<void()> animationFunc);
        void setAnimationEndFunc(function<void()> animationEndFunc);
        void setMoveAnimation(const Vector2 moveDest, const float duration);
        void setScaleAnimation(const float newScale, const float duration);
        void setFadeAnimation(const float newOpacity, const float duration);
        void start();
        void pause();
        void end();
        void play();
};