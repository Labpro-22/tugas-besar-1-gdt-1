#pragma once
#include "CardView.hpp"
#include "../../../models/CardAndDeck/CardDeck.hpp"
#include <queue>

class CardPileView {
    private:
        Vector3 pos;
        Vector2 cardSize;
        CardDeck<Card>& cardPile;
        ActionCardView* drawnCard;
        vector<ActionCardView*> cards;
        BoardView* board;
        CardCategory category;
    public:
        CardPileView(CardDeck<Card>& cardPile, const Vector3& pos, const Vector2& cardSize);
        CardCategory getPileCategory();
        Vector3 getPos();
        Vector2 getCardSize();
        CardDeck<Card>& getDeck();
        void updatePile();
        void drawCard();
        void render();
};