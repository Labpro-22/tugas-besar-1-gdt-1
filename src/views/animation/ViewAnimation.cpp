#include "../include/views/animation/ViewAnimation.hpp"

ViewAnimation::ViewAnimation(View2D& view, const float fps, bool interuptable, function<void()> animationFunc,  function<void()> animationEndFunc) : 
    view(view), fps(fps), playing(false), ended(false), frameProgress(0), 
    interuptable(interuptable), animationFunc(animationFunc), animationEndFunc(animationEndFunc) {}



const bool ViewAnimation::isPlaying() const { return playing; }
const bool ViewAnimation::hasEnded() const { return ended; }
const bool ViewAnimation::isInteruptable() const { return interuptable; }


void ViewAnimation::setAnimationFunc(function<void()> animationFunc) { this->animationFunc = animationFunc; }
void ViewAnimation::setAnimationEndFunc(function<void()> animationEndFunc) { this->animationEndFunc = animationEndFunc; }
void ViewAnimation::setMoveAnimation(const Vector2 moveDest, const float duration) {
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

void ViewAnimation::setScaleAnimation(const float newScale, const float duration) {
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

void ViewAnimation::setFadeAnimation(const float newOpacity, const float duration) {
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