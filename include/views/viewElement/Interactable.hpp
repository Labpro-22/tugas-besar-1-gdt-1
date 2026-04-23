#pragma once
#include "View2D.hpp"

class Interactable : public View2D {
    protected:
        Vector2 hitboxDim;
        bool active;
        bool draggable;
        bool releaseCommand;
        string gameCommand;
        function<void()> onHoverFunc;
        function<void()> onClickedFunc;
    public:
        Interactable();
        Interactable(const Vector2& boundingDim, bool active, bool draggable, string gameCommand, function<void()> onHoverFunc, function<void()> onClickedFunc);
        const bool isActive() const;
        const bool isDraggable() const;
        const string getGameCommand() const;
        const Rectangle getHitbox() const;
        void setActive(bool active);
        void setDraggable(bool draggable);
        void setGameCommand(string gameCommand);
        void setHitboxDim(const Vector2& hitbox);
        void setOnHover(function<void()> onHover);
        void setOnClicked(function<void()> onClicked);
        const bool isInBoundingBox(float x, float y) const;
        const bool isInBoundingBox(const Vector2& v) const;
        virtual void enable() override;
        virtual void disable() override;
        virtual void interactionCheck() override;
        virtual const string catchCommand() override;
        virtual void onDrag(const Vector2& deltaPos);
        virtual void onDefault();
        virtual void onHover();
        virtual void onClicked();
        virtual void onPressed();
        
        
};