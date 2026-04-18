#include "Entry.hpp"

int Entry::textCursorTick = 0;

Entry::Entry() : Interactable(), defaultText(""), fontSize(0), onEnterFunc([](){}), selected(false) {}

Entry::Entry(const Vector2& recDim, const string defaultText, const float fontSize, string fontKey, function<void()> onEnter) :
    Interactable(recDim, true, false, "NULL", [](){}, [](){}),
    defaultText(defaultText), entryText(""), fontSize(fontSize), fontKey(fontKey), onEnterFunc(onEnter), selected(false) {
        renderFunc = [this](){
            Vector2 renderPos = getRenderPos();
            DrawRectangle(renderPos.x, renderPos.y, this->getRenderWidth(), getRenderHeight(), WHITE);
            if (!selected && entryText.length() == 0) {
                DrawTextEx(fontMap[this->fontKey], this->defaultText.c_str(), 
                          {renderPos.x*1.05f, pos.y - getRenderFontSize(this->fontSize)/2}, getRenderFontSize(this->fontSize), 1, BLACK);
            } else {
                DrawTextEx(fontMap[this->fontKey], this->entryText.c_str(), 
                         {renderPos.x*1.05f, pos.y - getRenderFontSize(this->fontSize)/2}, getRenderFontSize(this->fontSize), 1,  BLACK);
                if (selected && textCursorTick % GetFPS()*2 < GetFPS()) {
                    float textCursorX = renderPos.x*1.055 + MeasureTextEx(fontMap[this->fontKey], this->entryText.c_str(), getRenderFontSize(this->fontSize), 1).x;
                    DrawLine(textCursorX, pos.y - getRenderFontSize(this->fontSize)/2, 
                             textCursorX, pos.y + getRenderFontSize(this->fontSize)/2, BLACK);
                }
            }
            if (selected) {
                textCursorTick++;
                if (textCursorTick % GetFPS()*2  == 0) textCursorTick = 0;

                DrawRectangleLinesEx({renderPos.x, renderPos.y, this->getRenderWidth(), getRenderHeight()}, 3, LIME);
            }
        };
}

const string Entry::getEntryText() const {
    return entryText;
}

const bool Entry::isSelected() const {
    return selected;
}

void Entry::setDefaultText(const string text) {
    defaultText = text;
}

void Entry::setFontSize(const float fontsize) {
    fontSize = fontsize;
}

void Entry::setOnEnterFunc(function<void()> onEnter) {
    onEnterFunc = onEnter;
}

void Entry::onHover() {
    onHoverFunc();
}

void Entry::onClicked() {
    selected = true;
    onClickedFunc();
}

void Entry::onEnter() {
    releaseCommand = true;
    onEnterFunc();
}
bool IsAnyKeyPressed()
{
    bool keyPressed = false;
    int key = GetKeyPressed();

    if (((key >= 32) && (key <= 126)) || key == KEY_BACKSPACE || key == KEY_ENTER) keyPressed = true;

    return keyPressed;
}


void Entry::interactionCheck() {
    if (active) {
        if (isInBoundingBox(GetMousePosition())) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                onClicked();
            } else {
                onHover();
            }
        } else {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                selected = false;
            }
        }
        if (selected) {
            if (IsAnyKeyPressed()) {
                int key = GetCharPressed();
                if (IsKeyDown(KEY_BACKSPACE)) {
                    if (entryText.length() > 0) {
                        entryText = entryText.substr(0, entryText.length() - 1);
                    }
                } else if (IsKeyPressed(KEY_ENTER)) {
                    onEnter();
                } else {
                    entryText += key;
                }
            }
        }
    }
}




