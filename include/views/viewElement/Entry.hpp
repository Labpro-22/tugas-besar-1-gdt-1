#pragma once
#include "Interactable.hpp"

class Entry : public Interactable {
    private :
        string defaultText;
        string entryText;
        float fontSize;
        string fontKey;
        bool selected;
        function<void()> onEnterFunc;
        static int textCursorTick;
    public :
        Entry();
        Entry(const Vector2& recDim, const string defaultText, const float fontSize, string fontKey, function<void()> onEnter);
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