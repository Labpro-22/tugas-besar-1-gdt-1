#include "../include/views/viewElement/AuctionMenuView.hpp"
#include "../include/views/animation/ViewAnimation.hpp"


AuctionMenuView::AuctionMenuView(Property* auctionedProperty, Game* game, Player* auctioner, vector<PlayerProfileView *> playerProfiles) :
    auctionedProperty(auctionedProperty), players(game->getActivePlayers()), auctioner(auctioner), highestBidder(nullptr),
    bidAmount(0), playerProfiles(playerProfiles),
    bidEntry(Entry((Vector2){GetScreenWidth()*0.3f, 50},"Enter Bid", 40, "Orbitron", [](){})),
    passButton(Interactable((Vector2){100, 50}, true, false, "NULL", [](){}, [](){})),
    MenuView(View2D(getScreenCenter(),{(float)GetScreenWidth(), (float)GetScreenHeight()}, [](){}))
    {
        passButton.setRender([this](){
            DrawRectangle(passButton.getPos().x, passButton.getPos().y, passButton.getRenderWidth(), passButton.getRenderHeight(),
                          getRenderColor(MAROON));
            drawTextLinesWrapped(fontMap.at("Orbitron"), "PASS", passButton.getPos(), 40, 1, getRenderColor(WHITE), passButton.getBoundingDim());
        });
        float startX = GetScreenWidth()/playerProfiles.size()*0.5f;
        float yPos = 0;
        for(int i = 0; i < playerProfiles.size(); i++) {
            if (playerProfiles[i]->getPos().y > yPos) {
                yPos = playerProfiles[i]->getPos().y;
            }
        }
        for(int i = 0; i < playerProfiles.size(); i++) {
            View2DAnimation* moveAnim = new View2DAnimation(*playerProfiles[i], 120, false, [](){}, [](){});
            moveAnim->setMoveAnimation({startX + i*GetScreenWidth()/playerProfiles.size(), yPos}, 0.3);
            moveAnim->start();
            playerProfiles[i]->addAnimation("MOVE_AUCTION", moveAnim);
        }
    }