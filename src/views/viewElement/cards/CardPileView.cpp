#include "../include/views/viewElement/cards/CardPileView.hpp"
#include "../include/views/animation/ViewAnimation.hpp"


CardPileView::CardPileView(CardDeck<Card>& cardPile, const Vector3& pos, const Vector2& cardSize) : 
    pos(pos), cardPile(cardPile), drawnCard(nullptr), cardSize(cardSize),
    category(cardPile.getDrawPile().at(0)->getCategory()) {
    int i = 0;
    for (Card* card : cardPile.getDrawPile()) {
        cards.push_back(new CardView(this, cardSize, {pos.x, pos.y + i*0.1f, pos.z}));
        i++;
    }
}

CardCategory CardPileView::getPileCategory() {
    return category;
}

Vector3 CardPileView::getPos() {
    return pos;
}

void CardPileView::drawCard() {
    drawnCard = cards.back();
    drawnCard->renderContent(*cardPile.getDrawPile().back());
    cards.pop_back();
    int neg = cardSize.x > 0 ? -1 : 3;
    View3DAnimation* drawAnim1 = new View3DAnimation(*drawnCard, 120, false, [](){}, [](){});
    drawAnim1->setMoveAnimation(drawnCard->getPos() + (Vector3){0,2.0f,0}, 0.2);
    drawAnim1->start();
    View3DAnimation* drawAnim2 = new View3DAnimation(*drawnCard, 120, false, [](){}, [this](){
        int neg = cardSize.x > 0 ? -3 : 3;
        View3DAnimation* drawAnim3 = new View3DAnimation(*drawnCard, 120, false, [](){}, [](){});
        drawAnim3->setMoveAnimation({-15.0f, 12.0f, 0}, 0.2);
        drawAnim3->start();
        View3DAnimation* drawAnim4 = new View3DAnimation(*drawnCard, 120, false, [](){}, [](){});
        drawAnim4->setRotateAnimation(neg*43.0f, {1.0,0,0}, 0.2);
        drawAnim4->start();
        drawnCard->addAnimation("DRAW_3", drawAnim3);
        drawnCard->addAnimation("DRAW_4", drawAnim4);
    });
    drawAnim2->setRotateAnimation(neg*45, {0,1.0,0}, 0.2);
    drawAnim2->start();
    drawnCard->addAnimation("DRAW_1", drawAnim1);
    drawnCard->addAnimation("DRAW_2", drawAnim2);
}

void CardPileView::render() {
    for (CardView* card : cards) {
        card->render();
    }
    if (drawnCard != nullptr) drawnCard->render();
}
