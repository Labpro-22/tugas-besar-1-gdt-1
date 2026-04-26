#include "../include/views/viewElement/AuctionMenuView.hpp"
#include "../include/views/animation/ViewAnimation.hpp"


AuctionMenuView::AuctionMenuView(Property* auctionedProperty, Game* game, Player* auctioner, vector<PlayerProfileView *> playerProfiles) :
    auctionedProperty(auctionedProperty), players(game->getActivePlayers()), auctioner(auctioner), highestBidder(nullptr),
    bidAmount(0), playerProfiles(playerProfiles),
    bidEntry(Entry((Vector2){120, 60},"Enter Bid", 32, "Orbitron", [](){})),
    passButton(Interactable((Vector2){280, 60}, true, false, "PASS", [](){}, [](){})),
    bidButton(Interactable((Vector2){120, 60}, true, false, "BID", [](){}, [](){})),
    MenuView(View2D(getScreenCenter(),{(float)GetScreenWidth(), (float)GetScreenHeight()}, [](){}))
    {
        
        passButton.setRender([this](){
            DrawRectangleRounded({passButton.getRenderPos().x, passButton.getRenderPos().y, passButton.getRenderWidth(), passButton.getRenderHeight()},
                                0.2f, 8, getRenderColor(MAROON));
            drawTextLinesWrapped(GetFontDefault(), "Pass", passButton.getPos(), 32, 1, getRenderColor(WHITE), passButton.getBoundingDim());
        });
        bidButton.setRender([this](){
            DrawRectangleRounded({bidButton.getRenderPos().x, bidButton.getRenderPos().y, bidButton.getRenderWidth(), bidButton.getRenderHeight()},
                                0.2f, 8, getRenderColor({111,111,145,255}));
            drawTextLinesWrapped(GetFontDefault(), "Bid", bidButton.getPos(), 32, 1, getRenderColor(WHITE), bidButton.getBoundingDim());
        });
        passButton.setPosition({GetScreenWidth()/2.0f, GetScreenHeight()*0.75f});
        bidEntry.setPosition({passButton.getPos().x - passButton.getRenderWidth()*0.25f, GetScreenHeight()*0.64f});
        bidButton.setPosition({passButton.getPos().x + passButton.getRenderWidth()*0.25f, GetScreenHeight()*0.64f});
        
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

void AuctionMenuView::initiateAuctionTurn(Player* currentBidder, bool forceBid) {
    if (forceBid) {
        passButton.disable();
    } else {
        passButton.enable();
    }
    this->currentBidder = currentBidder;
    int playerIdx = find_if(playerProfiles.begin(), playerProfiles.end(), [currentBidder](const PlayerProfileView* p){
        return p->getPlayer() == currentBidder;
    }) - playerProfiles.begin();
    float startX = GetScreenWidth()/playerProfiles.size()*0.5f;
    View2DAnimation* btnMove = new View2DAnimation(passButton, 120, false, [](){}, [](){});
    btnMove->setMoveAnimation({startX + playerIdx*GetScreenWidth()/playerProfiles.size(), passButton.getPos().y}, 0.2);
    btnMove->start();
    passButton.addAnimation("TURN_MOVE", btnMove);
    View2DAnimation* entryMove = new View2DAnimation(bidEntry, 120, false, [](){}, [](){});
    entryMove->setMoveAnimation({startX + playerIdx*GetScreenWidth()/playerProfiles.size() - passButton.getRenderWidth()*0.25f, 
                                bidEntry.getPos().y}, 0.2);
    entryMove->start();
    bidEntry.addAnimation("ENTRY_MOVE", entryMove);
    View2DAnimation* bidMove = new View2DAnimation(bidButton, 120, false, [](){}, [](){});
    bidMove->setMoveAnimation({startX + playerIdx*GetScreenWidth()/playerProfiles.size() + passButton.getRenderWidth()*0.25f, 
                                bidButton.getPos().y}, 0.2);
    bidMove->start();
    bidButton.addAnimation("TURN_MOVE", bidMove);
}

string AuctionMenuView::catchCommand() {
    string bid = bidButton.catchCommand();
    if (bid != "NULL") {
        return bid + " " + bidEntry.getEntryText();
    }

    string pass = passButton.catchCommand();
    if (pass != "NULL") {
        return pass;
    }
    return 
    "NULL";
}



void AuctionMenuView::updateAuction(Player* highestBidder, int highestBid) {
    this->highestBidder = highestBidder;
    int bidDiff = highestBid - bidAmount;
    View2DAnimation* bidRaise = new View2DAnimation(*this, 120, false, [this, highestBid, bidDiff](){
        if (bidAmount >= highestBid) {
            bidAmount = highestBid;
            this->getAnimation("BID_RAISE")->end();
        } else {
            bidAmount += 4;
        }
    }, [](){});
    bidRaise->start();
    addAnimation("BID_RAISE", bidRaise);
}

void AuctionMenuView::endAuction(Player* winner) {
    bidEntry.setVisible(false);
    bidButton.setVisible(false);
    passButton.setVisible(false);
    auto it = find_if(playerProfiles.begin(), playerProfiles.end(), [winner](const PlayerProfileView* p){
        return p->getPlayer() == winner;
    });
    PlayerProfileView* winnerProfile = *it;
    View2DAnimation* winMove = new View2DAnimation(*winnerProfile, 120, false, [](){}, [this, winnerProfile](){
        View2DAnimation* winWait = new View2DAnimation(*winnerProfile, 120, false, [](){}, [](){});
        winWait->setWait(2, [this](){
            View2DAnimation* menuLeave = new View2DAnimation(*this, 120, false, [](){}, [this](){
                closeView = true;
            });
            menuLeave->setMoveAnimation({0, GetScreenHeight()*-1.0f}, 0.3);
            menuLeave->start();
            addAnimation("MENU_LEAVE", menuLeave);
            float wProfile = 250.0f;
            float hProfile = 80.0f;
            float margin = 20.0f;
            Vector2 corners[4] = {
                {wProfile / 2 + margin, hProfile / 2 + margin},                                       // TL
                {GetScreenWidth() - wProfile / 2 - margin, hProfile / 2 + margin},                    // TR
                {wProfile / 2 + margin, GetScreenHeight() - hProfile / 2 - margin},                   // BL
                {GetScreenWidth() - wProfile / 2 - margin, GetScreenHeight() - hProfile / 2 - margin} // BR
            };

            for(int i = 0; i < playerProfiles.size(); i++) {
                View2DAnimation* moveAnim = new View2DAnimation(*playerProfiles[i], 120, false, [](){}, [](){});
                moveAnim->setMoveAnimation(corners[i], 0.3);
                moveAnim->start();
                playerProfiles[i]->addAnimation("MOVE_BACK", moveAnim);
            }
        });
        winWait->start();
        addAnimation("WAIT", winWait);
    });
    winMove->setMoveAnimation({GetScreenWidth()/2.0f, GetScreenHeight()/2.0f}, 1);
    winMove->start();
    addAnimation("MOVE_CENTER", winMove);
}

void AuctionMenuView::interactionCheck() {
    bidEntry.interactionCheck();
    bidButton.interactionCheck();
    passButton.interactionCheck();
}

void AuctionMenuView::render() {
    animationCheck();
    DrawRectangleRounded({pos.x - GetScreenWidth()*0.25f, pos.y - GetScreenHeight()*0.45f, GetScreenWidth()*0.5f, GetScreenHeight()*0.25f}, 
                         1.0f, 8, {11,23,40,255});
    string highestBidder = "None";
    if (this->highestBidder != nullptr) {
        highestBidder = this->highestBidder->getUsername();
    }
    DrawTextEx(fontMap.at("Kabel"), ((string)("Current Bid : M" + to_string(bidAmount))).c_str(), 
                                 {pos.x - GetScreenWidth()*0.18f, pos.y - GetScreenHeight()*0.4f}, 36, 1, WHITE);
    DrawTextEx(fontMap.at("Kabel"), ((string)("Highest Bidder : " + highestBidder)).c_str(), 
                                 {pos.x - GetScreenWidth()*0.18f, pos.y - GetScreenHeight()*0.3f}, 36, 1, WHITE);
    bidEntry.render();
    bidButton.render();
    passButton.render();
}