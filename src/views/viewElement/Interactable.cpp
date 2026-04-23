#include "views/viewElement/Interactable.hpp"


Interactable::Interactable() :
    hitboxDim({0,0}), active(false), draggable(false), releaseCommand(false), gameCommand("NULL"), onHoverFunc([](){}), onClickedFunc([](){}) {}

Interactable::Interactable(const Vector2& boundingDim, bool active, bool draggable, string gameCommand, function<void()> onHoverFunc, function<void()> onClickedFunc) :
    View2D({0,0}, boundingDim, [](){}),
    hitboxDim(boundingDim), active(active), draggable(draggable), releaseCommand(false),
    gameCommand(gameCommand), onHoverFunc(onHoverFunc), onClickedFunc(onClickedFunc) {}


const bool Interactable::isActive() const { return active; }
const bool Interactable::isDraggable() const { return draggable; }
const string Interactable::getGameCommand() const { return gameCommand; }
const Rectangle Interactable::getHitbox() const { 
    return {pos.x - hitboxDim.x/2, pos.y - hitboxDim.y/2 , hitboxDim.x, hitboxDim.y}; 
}
void Interactable::setActive(bool active) { this->active = active; }
void Interactable::setDraggable(bool draggable) { this->draggable = draggable; }
void Interactable::setGameCommand(string gameCommand) { this->gameCommand = gameCommand; }
void Interactable::setHitboxDim(const Vector2& hitbox) { this->hitboxDim = hitbox; }
void Interactable::setOnHover(function<void()> onHover) { onHoverFunc = onHover; }
void Interactable::setOnClicked(function<void()> onClicked) { onClickedFunc = onClicked; }

const bool Interactable::isInBoundingBox(float x, float y) const {
    return CheckCollisionPointRec({x,y}, getHitbox());
}
const bool Interactable::isInBoundingBox(const Vector2& v) const {
    return CheckCollisionPointRec(v, getHitbox());
}

void Interactable::onDrag(const Vector2& deltaPos) {
    movePositionDelta(deltaPos);
}

void Interactable::onDefault() {
    brightness = 0;
    scale = 1;
}

void Interactable::onHover() {
    brightness = 0.1;
    scale = 1.02;
    onHoverFunc();
}
void Interactable::onClicked() {
    releaseCommand = true;
    brightness = -0.2;
    scale = 0.98;
    onClickedFunc();
}

void Interactable::onPressed() {
    brightness = -0.2;
    scale = 0.98;
}

void Interactable::enable() {
    active = true;
}

void Interactable::disable() {
    active = false;
}

const string Interactable::catchCommand() {
    if (releaseCommand) {
        releaseCommand = false;
        return gameCommand;
    } else {
        return "NULL";
    }
}

void Interactable::interactionCheck() {
    if (active) {
        onDefault();
        if (isInBoundingBox(GetMousePosition())) {
            
            if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                onClicked();
            } 
            else if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                onPressed();
            }
            else {
                onHover();
            }

            
            
            if (draggable && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                onDrag(GetMouseDelta());
            }
        }
    }
}


