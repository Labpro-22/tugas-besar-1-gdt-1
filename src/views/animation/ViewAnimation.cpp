#include "../include/views/animation/ViewAnimation.hpp"

ViewAnimation::ViewAnimation(const float fps, bool interuptable, function<void()> animationFunc,  function<void()> animationEndFunc) :
    fps(fps), interuptable(interuptable), playing(false), ended(false), frameProgress(0), 
    animationFunc(animationFunc), animationEndFunc(animationEndFunc) {}

View2DAnimation::View2DAnimation(View2D& view, const float fps, bool interuptable, function<void()> animationFunc,  function<void()> animationEndFunc) : 
    view(view), ViewAnimation(fps, interuptable, animationFunc, animationEndFunc) {}

View3DAnimation::View3DAnimation(View3D& view, const float fps, bool interuptable, function<void()> animationFunc,  function<void()> animationEndFunc) : 
    view(view), ViewAnimation(fps, interuptable, animationFunc, animationEndFunc) {}

const bool ViewAnimation::isPlaying() const { return playing; }
const bool ViewAnimation::hasEnded() const { return ended; }
const bool ViewAnimation::isInteruptable() const { return interuptable; }


void ViewAnimation::setAnimationFunc(function<void()> animationFunc) { this->animationFunc = animationFunc; }
void ViewAnimation::setAnimationEndFunc(function<void()> animationEndFunc) { this->animationEndFunc = animationEndFunc; }
void View2DAnimation::setMoveAnimation(const Vector2 moveDest, const float duration) {
    Vector2 startingPos = view.getPos();
    animationFunc = [moveDest, duration, startingPos, this](){
        if (frameProgress/fps >= duration) {
            view.movePosition(moveDest);
            ended = true;
        } else {
            view.movePosition(startingPos + (moveDest - startingPos)*((frameProgress/fps)/duration));
        }
    };
}

void View3DAnimation::setMoveAnimation(const Vector3 moveDest, const float duration) {
    Vector3 startingPos = view.getPos();
    animationFunc = [moveDest, duration, startingPos, this](){
        if (frameProgress/fps >= duration) {
            view.movePosition(moveDest);
            ended = true;
        } else {
            view.movePosition(startingPos + (moveDest - startingPos)*((frameProgress/fps)/duration));
        }
    };
}

void View3DAnimation::setMoveYAnimation(float moveDest, const float duration) {
    float startingY = view.getPos().y;
    animationFunc = [moveDest, startingY, duration, this](){
        if (frameProgress/fps >= duration) {
            view.setPosY(moveDest);
            ended = true;
        } else {
            view.setPosY(startingY + (moveDest - startingY)*((frameProgress/fps)/duration));
        }
    };
}

void View2DAnimation::setScaleAnimation(const float newScale, const float duration) {
    float startingScale = view.getScale();
    animationFunc = [newScale, duration, startingScale, this](){
        if (frameProgress/fps >= duration) {
            view.setScale(newScale);
            ended = true;
        } else {
            view.setScale(startingScale + (newScale - startingScale)*((frameProgress/fps)/duration));
        }
    };
}

void View2DAnimation::setFadeAnimation(const float newOpacity, const float duration) {
    float startingOpacity = view.getOpacity();
    animationFunc = [newOpacity, duration, startingOpacity, this](){
        if (frameProgress/fps >= duration) {
            view.setOpacity(newOpacity);
            ended = true;
        } else {
            view.setOpacity(startingOpacity + (newOpacity - startingOpacity)*((frameProgress/fps)/duration));
        }
    };
}

void ViewAnimation::start() { playing = true; }
void ViewAnimation::pause() { playing = false; }
void ViewAnimation::end() { ended = true; }
void ViewAnimation::play() {
    if (playing) { 
        animationFunc(); 
        frameProgress++;
    }
    if (ended) { animationEndFunc(); }
    
}