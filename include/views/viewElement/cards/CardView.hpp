#pragma once
#include "../View3D.hpp"
#include "../board/BoardView.hpp"
#include "../../../models/CardAndDeck/Card.hpp"

class CardPileView;

class CardView : public View3D {
    protected :
        RenderTexture2D cardTexture;
        bool flipped;
        static map<CardCategory, Texture2D> cardTextureMap;
        static bool texturesLoaded;
        CardPileView* pile;
    public :
        CardView(CardPileView* pile, const Vector2& cardSize, const Vector3& pos);
        virtual void renderContent(Card& card);
        static void loadCardTextures();
};