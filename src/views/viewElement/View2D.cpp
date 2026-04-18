#include "../include/views/viewElement/View2D.hpp"

map<string, Font> View2D::fontMap;

View2D::View2D() :
    pos({0,0}), boundingDim({0,0}), scale(1), brightness(0), opacity(1), visible(true), renderFunc([](){}), closeView(false) {}

View2D::View2D(const Vector2& pos, const Vector2& boundingDim, function<void()> renderFunc) :
    pos(pos), boundingDim(boundingDim), scale(1), brightness(0), opacity(1), visible(true), renderFunc(renderFunc), closeView(false) {}

const float View2D::getX() const { return pos.x; }
const float View2D::getY() const { return pos.y; }
const Vector2 View2D::getPos() const { return pos; }
const Vector2 View2D::getRenderPos() const { return pos - getRenderDim()/2; }
const float View2D::getBoundingWidth() const { return boundingDim.x; }
const float View2D::getBoundingHeight() const { return boundingDim.y; }
const Vector2 View2D::getBoundingDim() const { return boundingDim; }
const float View2D::getRenderWidth() const { return (boundingDim*scale).x; }
const float View2D::getRenderHeight() const { return (boundingDim*scale).y; }
const Vector2 View2D::getRenderDim() const { return boundingDim*scale; }
const float View2D::getScale() const { return scale; }
const float View2D::getRenderFontSize(const float fontsize) const { return fontsize*scale; }
const float View2D::getBrightness() const { return brightness; }
const Color View2D::getRenderColor(const Color& color) const { return ColorBrightness(color, brightness); }
const bool View2D::isVisible() const { return visible; }
const bool View2D::closed() const { return closeView; }

void View2D::movePosition(float x, float y) { pos = {x,y}; }
void View2D::movePosition(const Vector2& v) { pos = v; }
void View2D::movePositionDelta(float dx, float dy) { pos += {dx,dy}; }
void View2D::movePositionDelta(const Vector2& dv) { pos += dv; }
void View2D::setScale(float scale) { this->scale = scale; }
void View2D::setVisible(bool visible) { this->visible = visible; }
void View2D::setRender(function<void()> renderFunc) { this->renderFunc = renderFunc; }


const string View2D::catchCommand() { return "NULL"; }
void View2D::render() {
    renderFunc();
}

void View2D::addFont(string fontKey, string fontFilename) {
    fontMap[fontKey] = LoadFontEx(fontFilename.c_str(), 36, NULL, 0);
}

void View2D::unloadFonts() {
    for (const auto& entry : fontMap) {
        UnloadFont(entry.second);
    }
}

const Vector2 getScreenCenter() {
    return {(float)GetScreenWidth()/2, (float)GetScreenHeight()/2};
}

const Vector2 getScreenDim() {
    return {(float)GetScreenWidth(), (float)GetScreenHeight()};
}

void drawTextLinesWrapped(Font font, const string text, Vector2 centerPos, float fontsize, float spacing, Color textColor, Vector2 boundingDim) {
    stringstream ss(text);
    string item;
    vector<string> pre_determined_lines;
    vector<string> textLines;
    vector<float> textLineFontSizes;
    vector<Vector2> textLinePos;
    
    while(getline(ss, item, '\n')) {
        pre_determined_lines.push_back(item);
    }
    for (string textLine : pre_determined_lines) {
        stringstream ss2(textLine);
        string item2;
        vector<string> words;
        while(getline(ss2, item2, ' ')) {
            words.push_back(item2);
        }
        
        string line = words[0] + " ";
        float lineFont = fontsize;
        for (int i = 1; i < words.size(); i++) {
            if (MeasureTextEx(font, (line + words[i]).c_str(), lineFont, 1).x < boundingDim.x) {
                line += words[i] + " ";
            } else {
                if (count(line.begin(), line.end(), ' ') == 1) {
                    while(MeasureTextEx(font, (line).c_str(), lineFont, 1).x >= boundingDim.x) {
                        lineFont--;
                    }
                }
                line = line.substr(0, line.length() - 1);
                textLines.push_back(line);
                textLineFontSizes.push_back(lineFont);
                line = words[i] + " ";
                lineFont = fontsize;
            }
        }
        if (count(line.begin(), line.end(), ' ') == 1) {
            while(MeasureTextEx(font, (line).c_str(), lineFont, 1).x >= boundingDim.x) {
                lineFont--;
            }
        }
        line = line.substr(0, line.length() - 1);
        textLines.push_back(line);
        textLineFontSizes.push_back(lineFont);
    }

    float midPointY = accumulate(textLineFontSizes.begin(), textLineFontSizes.end(), 0)/2;
    float currentY = 0;
    for (int i = 0; i < textLines.size(); i++) {
        Vector2 textDim = MeasureTextEx(font, textLines[i].c_str(), textLineFontSizes[i], 1);
        textLinePos.push_back(centerPos + (Vector2){-textDim.x/2, currentY - midPointY});
        currentY += textLineFontSizes[i];
    }

    for (int i = 0; i < textLines.size(); i++) {
        DrawTextEx(font, textLines[i].c_str(), textLinePos[i], textLineFontSizes[i], 1, textColor);
    }
}