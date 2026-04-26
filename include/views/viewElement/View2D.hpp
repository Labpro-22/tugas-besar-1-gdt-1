#pragma once
#include "raylib.h"
#include "raymath.h"
#include "../ColorPallete.hpp"
#include <functional>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <sstream>
using namespace std;

class View2DAnimation;

class View2D {
    protected:
        Vector2 pos;
        float scale;
        float brightness;
        float opacity;
        bool visible;
        Vector2 boundingDim;
        function<void()> renderFunc;
        static map<string, Font> fontMap;
        bool closeView;
        map<string, View2DAnimation*> animations;
    public:
        View2D();
        View2D(const Vector2& pos, const Vector2& boundingDim, function<void()> renderFunc);
        virtual ~View2D();
        const float getX() const;
        const float getY() const;
        const Vector2 getPos() const;
        const Vector2 getRenderPos() const;
        const float getBoundingWidth() const;
        const float getBoundingHeight() const;
        const Vector2 getBoundingDim() const;
        const float getRenderWidth() const;
        const float getRenderHeight() const;
        const Vector2 getRenderDim() const;
        const float getScale() const;
        const float getRenderFontSize(const float fontsize) const;
        const float getBrightness() const;
        const float getOpacity() const;
        const Color getRenderColor(const Color& color) const;
        const bool isVisible() const;
        const bool closed() const;
        const Vector2& getPosition() const;
        
        void setPosition(const Vector2& v);
        virtual void movePosition(const Vector2& v);
        void movePosition(float x, float y);
        void movePositionDelta(const Vector2& dv);
        void movePositionDelta(float dx, float dy);
        void setScale(float scale);
        void setBrightness(float brightness);
        void setOpacity(float opacity);
        void setVisible(bool visible);
        virtual void close();
        void setRender(function<void()> renderFunc);
        void addAnimation(std::string animKey, View2DAnimation* anim);
        View2DAnimation* getAnimation(std::string animKey) const;
        const bool isAnimationActive() const;
        void animationCheck();
        virtual void enable() {};
        virtual void disable() {};
        virtual void interactionCheck() {};
        virtual std::string catchCommand();
        virtual void render();
        static void addFont(string fontKey, string fontFilename);
        static Font getFont(string fontKey);
        static void unloadFonts();
};

const Vector2 getScreenCenter();

const Vector2 getScreenDim();

void drawTextLinesWrapped(Font font, const string text, Vector2 centerPos, float fontsize, float spacing, Color textColor, Vector2 boundingDim);