#pragma once
#include "Interactable.hpp"
#include <functional>
#include <string>

using namespace std;

class Entry : public Interactable
{
private:
    string defaultText;
    string entryText;
    float fontSize;
    string fontKey;
    bool selected;
    function<void()> onEnterFunc;

    int cursorPos = 0;
    bool cursorVisible = true;
    float cursorBlinkTime = 0.0f;

    float backspaceHoldTime = 0.0f;
    float backspaceRepeatTimer = 0.0f;
    float leftHoldTime = 0.0f;
    float leftRepeatTimer = 0.0f;
    float rightHoldTime = 0.0f;
    float rightRepeatTimer = 0.0f;

public:
    Entry();
    Entry(const Vector2 &recDim, const string defaultText, const float fontSize, string fontKey, function<void()> onEnter);
    const bool isSelected() const;
    const string getEntryText() const;
    void setDefaultText(const string text);
    void setFontSize(const float fontsize);
    void setOnEnterFunc(function<void()> onEnter);
    void onEnter();
    void onHover() override;
    void onClicked() override;
    void interactionCheck() override;
};