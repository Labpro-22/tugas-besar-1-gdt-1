#include "../include/views/viewElement/cards/CardPileView.hpp"
#include "../include/views/animation/ViewAnimation.hpp"


CardPileView::CardPileView(CardDeck<Card>& cardPile, const Vector3& pos, const Vector2& cardSize) : 
    pos(pos), cardPile(cardPile), drawnCard(nullptr), cardSize(cardSize),
    category(cardPile.getDrawPile().at(0)->getCategory()) {
    int i = 0;
    for (Card* card : cardPile.getDrawPile()) {
        cards.push_back(new ActionCardView(card, this, {pos.x, pos.y + i*0.1f, pos.z}));
        i++;
    }
}

CardDeck<Card>& CardPileView::getDeck() {
    return cardPile;
}

CardCategory CardPileView::getPileCategory() {
    return category;
}

Vector3 CardPileView::getPos() {
    return pos;
}

Vector2 CardPileView::getCardSize() {
    return cardSize;
}

void CardPileView::updatePile() {
    int i = 0;
    vector<ActionCardView*> newPile;
    for(Card* card : cardPile.getDrawPile()) {
        auto it = find_if(cards.begin(), cards.end(), [card](const ActionCardView* cv) {
            return cv->getCard() == card;
        });
        if (it != cards.end()) {
            newPile.push_back(*it);
            (*it)->movePosition({pos.x, pos.y + (float)i*0.1f, pos.z});
            cards.erase(it);
            i++;
        }
    }
    cards = newPile;
    cout<<cards.size()<<endl;
}

void CardPileView::drawCard() {
    drawnCard = cards.back();
    Matrix original = drawnCard->getTransformation();
    cards.pop_back();
    int neg = cardSize.x > 0 ? -1 : 3;
    View3DAnimation* drawAnim1 = new View3DAnimation(*drawnCard, 120, false, [](){}, [](){});
    drawAnim1->setMoveAnimation(drawnCard->getPos() + (Vector3){0,2.0f,0}, 0.2);
    drawAnim1->start();
    View3DAnimation* drawAnim2 = new View3DAnimation(*drawnCard, 120, false, [](){}, [this, original](){
        int neg = cardSize.x > 0 ? -3 : 3;
        View3DAnimation* drawAnim3 = new View3DAnimation(*drawnCard, 120, false, [](){}, [](){});
        drawAnim3->setMoveAnimation({-15.0f, 12.0f, 0}, 0.2);
        drawAnim3->start();
        View3DAnimation* drawAnim4 = new View3DAnimation(*drawnCard, 120, false, [](){}, [this, original](){
            View3DAnimation* drawAnim5 = new View3DAnimation(*drawnCard, 120, false, [](){}, [](){});
            drawAnim5->setWait(2, [this, original](){
                int neg = cardSize.x > 0 ? 1 : -1;
                View3DAnimation* drawAnim6 = new View3DAnimation(*drawnCard, 120, false, [](){}, [this, original](){
                    int neg = cardSize.x > 0 ? 1 : -1;
                    drawnCard->setTransform(original);
                    drawnCard->movePosition(pos + (Vector3){-15, 0.1, (float) -15*neg});
                    for (ActionCardView* cardInPile : cards) {
                        View3DAnimation* drawAnim8 = new View3DAnimation(*cardInPile, 120, false, [](){}, [](){});
                        drawAnim8->setMoveAnimation(cardInPile->getPos() + (Vector3){0,2,0}, 0.2);
                        drawAnim8->start();
                        cardInPile->addAnimation("DRAW_8", drawAnim8);
                    }
                    View3DAnimation* drawAnim9 = new View3DAnimation(*drawnCard, 120, false, [](){}, [](){});
                    drawAnim9->setWait(0.2, [this](){
                        View3DAnimation* drawAnim7 = new View3DAnimation(*drawnCard, 120, false, [](){}, [this](){
                            for (ActionCardView* cardInPile : cards) {
                                View3DAnimation* drawAnim10 = new View3DAnimation(*cardInPile, 120, false, [](){}, [this](){});
                                drawAnim10->setMoveAnimation(cardInPile->getPos() - (Vector3){0,1.9f,0}, 0.2);
                                drawAnim10->start();
                                cardInPile->addAnimation("DRAW_10", drawAnim10);
                                cout<<pos.y<<endl;
                            }
                            cards.insert(cards.begin(), drawnCard);
                            View3DAnimation* drawAnim11 = new View3DAnimation(*drawnCard, 120, false, [](){}, [](){});
                            drawAnim11->setWait(0.3, [this](){ updatePile(); });
                            drawAnim11->start();
                            drawnCard->addAnimation("DRAW_11", drawAnim11);
                            drawnCard = nullptr;
                            
                        });
                        drawAnim7->setMoveAnimation(pos, 0.4);
                        drawAnim7->start();
                        drawnCard->addAnimation("DRAW_7", drawAnim7);
                    });
                    drawAnim9->start();
                    drawnCard->addAnimation("DRAW_9", drawAnim9);
                });
                drawAnim6->setMoveAnimation(drawnCard->getPos() + (Vector3){0,0,-20}*neg, 0.2);
                drawAnim6->start();
                drawnCard->addAnimation("DRAW_6", drawAnim6);
            });
            drawAnim5->start();
            drawnCard->addAnimation("DRAW_5", drawAnim5);
        });
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
    for (ActionCardView* card : cards) {
        card->render();
    }
    if (drawnCard != nullptr) drawnCard->render();
}
