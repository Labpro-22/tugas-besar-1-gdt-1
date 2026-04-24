#pragma once
#include "../View3D.hpp"
#include "../board/BoardView.hpp"
#include "../../../models/CardAndDeck/Card.hpp"

class CardPileView;

class CardView : public View3D {
    protected:
        Card* card;
    public :
        CardView(Card* card, const Vector2& cardSize, const Vector3& pos);
        Card* getCard() const;
};

class ActionCardView : public CardView {
    protected :
        CardPileView* pile;
        RenderTexture2D cardTexture;
    public :
        ActionCardView(Card* card, CardPileView* pile, const Vector3& pos);
        ~ActionCardView();
};

class SkillCardView : public CardView {
    protected :
        RenderTexture2D cardTexture;
    public :
        SkillCardView(Card* card, const Vector3& pos);
        ~SkillCardView();
};