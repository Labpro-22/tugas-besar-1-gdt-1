#pragma once
#include "../View3D.hpp"
#include "../board/BoardView.hpp"
#include "../../../models/CardAndDeck/Card.hpp"

class CardPileView;

class CardView : public View3D {
    protected :
        Card* card;
        RenderTexture2D cardTexture;
        CardPileView* pile;
    public :
        CardView(Card* card, CardPileView* pile, const Vector2& cardSize, const Vector3& pos);
        ~CardView();
        Card* getCard() const;
};