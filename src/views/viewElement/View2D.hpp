#pragma once
#include "raylib.h"
#include "raymath.h"
#include <functional>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
using namespace std;

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
    public:
        View2D();
        View2D(const Vector2& pos, const Vector2& boundingDim, function<void()> renderFunc);
        virtual ~View2D() {}
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
        const Color getRenderColor(const Color& color) const;
        const bool isVisible() const;
        const bool closed() const;
        void movePosition(float x, float y);
        void movePosition(const Vector2& v);
        void movePositionDelta(float dx, float dy);
        void movePositionDelta(const Vector2& dv);
        void setScale(float scale);
        void setBrightness(float brightness);
        void setVisible(bool visible);
        void setRender(function<void()> renderFunc);
        virtual void enable() {};
        virtual void disable() {};
        virtual void interactionCheck() {};
        virtual const string catchCommand();
        virtual void render();
        static void addFont(string fontKey, string fontFilename);
        static void unloadFonts();
};

const Vector2 getScreenCenter();

const Vector2 getScreenDim();

void drawTextLinesWrapped(Font font, const string text, Vector2 centerPos, float fontsize, float spacing, Color textColor, Vector2 boundingDim);