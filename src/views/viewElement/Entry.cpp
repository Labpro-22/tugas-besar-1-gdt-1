#include "views/viewElement/Entry.hpp"

Entry::Entry() : Interactable(), defaultText(""), fontSize(0), onEnterFunc([]() {}), selected(false) {}

Entry::Entry(const Vector2 &recDim, const string defaultText, const float fontSize, string fontKey, function<void()> onEnter)
    : Interactable(recDim, true, false, "NULL", []() {}, []() {}), defaultText(defaultText), entryText(""), fontSize(fontSize), fontKey(fontKey), onEnterFunc(onEnter), selected(false)
{
    renderFunc = [this]()
    {
        Vector2 renderPos = getRenderPos();
        float fontSizeRender = getRenderFontSize(this->fontSize);

        float textX = renderPos.x + 5;
        float textY = renderPos.y + (getRenderHeight() - fontSizeRender) / 2;

        DrawRectangle(renderPos.x, renderPos.y,
                      getRenderWidth(), getRenderHeight(), WHITE);

        if (!selected && entryText.empty())
        {
            DrawTextEx(fontMap[this->fontKey],
                       this->defaultText.c_str(),
                       {textX, textY},
                       fontSizeRender, 1, GRAY);
        }
        else
        {
            DrawTextEx(fontMap[this->fontKey],
                       this->entryText.c_str(),
                       {textX, textY},
                       fontSizeRender, 1, BLACK);
        }

        cursorBlinkTime += GetFrameTime();

        if (cursorBlinkTime >= 0.5f)
        {
            cursorVisible = !cursorVisible;
            cursorBlinkTime = 0.0f;
        }

        if (selected && cursorVisible)
        {
            std::string leftText = entryText.substr(0, cursorPos);

            float cursorX = textX +
                            MeasureTextEx(fontMap[this->fontKey],
                                          leftText.c_str(),
                                          fontSizeRender,
                                          1)
                                .x;

            DrawLine(cursorX, textY, cursorX, textY + fontSizeRender, BLACK);
        }

        if (selected)
        {
            DrawRectangleLinesEx(
                {renderPos.x, renderPos.y, getRenderWidth(), getRenderHeight()},
                3, LIME);
        }
    };
}

const string Entry::getEntryText() const { return entryText; }
const bool Entry::isSelected() const { return selected; }
void Entry::setDefaultText(const string text) { defaultText = text; }
void Entry::setFontSize(const float fontsize) { fontSize = fontsize; }
void Entry::setOnEnterFunc(function<void()> onEnter) { onEnterFunc = onEnter; }
void Entry::onHover() { onHoverFunc(); }

void Entry::onClicked()
{
    selected = true;
    onClickedFunc();
}

void Entry::onEnter()
{
    releaseCommand = true;
    onEnterFunc();
}

void Entry::interactionCheck()
{
    if (!active)
        return;

    if (isInBoundingBox(GetMousePosition()))
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            onClicked();
        else
            onHover();
    }
    else
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            selected = false;
    }

    if (selected)
    {
        float dt = GetFrameTime();

        int key = GetCharPressed();
        while (key > 0)
        {
            if ((key >= 32) && (key <= 125))
            {
                entryText.insert(cursorPos, 1, (char)key);
                cursorPos++;
                cursorVisible = true;
                cursorBlinkTime = 0.0f;
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_ENTER))
            onEnter();

        auto handleKeyRepeat = [&](int raylibKey, float &holdTime, float &repeatTimer, std::function<void()> action)
        {
            if (IsKeyDown(raylibKey))
            {
                holdTime += dt;
                repeatTimer += dt;

                if (IsKeyPressed(raylibKey))
                {
                    action();
                    repeatTimer = 0.0f;
                }
                else if (holdTime > 0.4f && repeatTimer > 0.05f)
                {
                    action();
                    repeatTimer = 0.0f;
                }
            }
            else
            {
                holdTime = 0.0f;
                repeatTimer = 0.0f;
            }
        };

        handleKeyRepeat(KEY_BACKSPACE, backspaceHoldTime, backspaceRepeatTimer, [&]()
                        {
            if (!entryText.empty() && cursorPos > 0) {
                entryText.erase(cursorPos - 1, 1);
                cursorPos--;
                cursorVisible = true;
                cursorBlinkTime = 0.0f;
            } });

        handleKeyRepeat(KEY_LEFT, leftHoldTime, leftRepeatTimer, [&]()
                        {
            if (cursorPos > 0) {
                cursorPos--;
                cursorVisible = true;
                cursorBlinkTime = 0.0f;
            } });

        handleKeyRepeat(KEY_RIGHT, rightHoldTime, rightRepeatTimer, [&]()
                        {
            if (cursorPos < entryText.length()) {
                cursorPos++;
                cursorVisible = true;
                cursorBlinkTime = 0.0f;
            } });
    }
}