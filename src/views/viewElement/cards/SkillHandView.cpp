#include "../include/views/viewElement/cards/SkillHandView.hpp"
#include "../include/views/animation/ViewAnimation.hpp"
#include "models/Player/Player.hpp"


SkillHandView::SkillHandView(Player& player, View3DCamera* cam, Card* incomingCard) : 
    player(player), incomingCard(nullptr), usedCard(nullptr), discardedCard(nullptr), 
    viewCam(cam), active(false), discarded(false), cardUsed(false),
    IndefinitePopup(View2D((Vector2){(float)GetScreenWidth()/2, (float)GetScreenHeight()}, (Vector2){(float)GetScreenWidth()/2,GetScreenHeight()*0.3f}, [](){})) {
    int i = 0;
    int cardAmount = player.getHandCards().size();
    Vector3 basePos = cam->getPos() + (cam->getTarget() - cam->getPos())*0.38f - (Vector3){0, 6.0f, 0};
    for (Card* skills : player.getHandCards()) {
        SkillCardView* skillCard = new SkillCardView(skills, basePos + (Vector3){0,0, -2.0f*(cardAmount - 1) + 4.0f*i});
        skillCard->transform(MatrixRotateZ(-M_PI/6));
        View3DAnimation* moveAnim = new View3DAnimation(*skillCard, 120, false, [](){}, [this](){
            if (this->incomingCard == nullptr) { enable(); }
        });
        skillCard->movePosition(skillCard->getPos() - (Vector3){5.0f, 2.5f, 0});
        moveAnim->setMoveAnimation(skillCard->getPos() + (Vector3){5.0f, 2.5f, 0}, 0.2);
        moveAnim->start();
        skillCard->addAnimation("INITIAL_MOVE", moveAnim);

        hand.push_back(skillCard);
        i++;
    }

    Vector2 buttonSize = MeasureTextEx(fontMap.at("Orbitron"), "Cancel", 36, 1)*1.5f;
    exitButton = Interactable(buttonSize, true, false, "DISPLAY EXIT_HAND", [](){}, [](){});
    exitButton.movePosition({GetScreenWidth()*0.9f, GetScreenHeight()*0.75f});
    exitButton.setRender([this](){
        DrawRectangle(exitButton.getRenderPos().x, exitButton.getRenderPos().y,
                    exitButton.getRenderWidth(), exitButton.getRenderHeight(), exitButton.getRenderColor(RED));
        Vector2 textDim = MeasureTextEx(fontMap.at("Orbitron"), "Cancel",
                                        exitButton.getRenderFontSize(36), 1);
        DrawTextEx(fontMap.at("Orbitron"), "Cancel", {exitButton.getX() - textDim.x/2, exitButton.getY() - 18}, 
                              getRenderFontSize(36), 1, getRenderColor(WHITE));
    });

    if (incomingCard != nullptr) {
        this->incomingCard = new SkillCardView(incomingCard, basePos + (Vector3){7.0f, 20.0f, 0});
        this->incomingCard->transform(MatrixRotateZ(-M_PI/2));
        View3DAnimation* descendAnim = new View3DAnimation(*this->incomingCard, 120, false, [](){}, [](){});
        descendAnim->setMoveAnimation(this->incomingCard->getPos() - (Vector3){0, 17.0f, 0}, 2);
        descendAnim->start();
        this->incomingCard->addAnimation("DESCEND_1A", descendAnim);
        View3DAnimation* descendRotateAnim = new View3DAnimation(*this->incomingCard, 120, false, [](){}, [this](){
            View3DAnimation* adjustAnim = new View3DAnimation(*this->incomingCard, 120, false, [](){}, [this](){
                if (this->hand.size() < 3) {
                    hand.push_back(this->incomingCard);
                    this->incomingCard = nullptr;
                    adjustHand();
                } else {
                    enable();
                }
            });
            adjustAnim->setRotateAnimation(30, {0,0,1}, 0.1);
            adjustAnim->start();
            this->incomingCard->addAnimation("DESCEND_2", adjustAnim);
        });
        descendRotateAnim->setRotateAnimation(720, {1,0,0}, 2);
        descendRotateAnim->start();
        this->incomingCard->addAnimation("DESCEND_1B", descendRotateAnim);
        exitButton.setVisible(false);
        exitButton.setActive(false);
    }
}

SkillHandView::~SkillHandView() {
    for (SkillCardView* skills : hand) {
        if (skills != nullptr) {
            delete skills;
        }
    }
    if (usedCard != nullptr) delete usedCard;
    if (incomingCard != nullptr) delete incomingCard;
    if (discardedCard != nullptr) delete discardedCard;
}

const bool SkillHandView::isAnimationActive() const {
    int found = false;
    for (SkillCardView* skill : hand) {
        if (skill != nullptr) found = skill->isAnimationActive();
    }
    if (incomingCard != nullptr) found = incomingCard->isAnimationActive();
    if (discardedCard != nullptr) found = discardedCard->isAnimationActive();
    if (usedCard != nullptr) found = usedCard->isAnimationActive();
    return found;
}

void SkillHandView::adjustHand() {
    disable();
    int i = 0;
    int cardAmount = hand.size();
    Vector3 basePos = viewCam->getPos() + (viewCam->getTarget() - viewCam->getPos())*0.38f - (Vector3){0, 6.0f, 0};
    if (hand.size() == 0) {close(); return;}
    for (SkillCardView* skills : hand) {
        View3DAnimation* adjustAnim = new View3DAnimation(*skills, 120, false, [](){}, [this](){ 
            enable();
            if (incomingCard == nullptr) {
                exitButton.setVisible(true);
                exitButton.setActive(true);
            } 
            if (usedCard != nullptr) {
                close();
            }
        });
        adjustAnim->setMoveAnimation(basePos + (Vector3){0,0, -2.0f*(cardAmount - 1) + 4.0f*i}, 0.2);
        adjustAnim->start();
        skills->addAnimation("ADJUST", adjustAnim);
        i++;
    }
}

void SkillHandView::discardCard(SkillCardView* skill) {
    disable();
    View3DAnimation* moveAnim1 = new View3DAnimation(*skill, 120, false, [](){}, [this, skill](){
        enable();
        discardedCard = skill;
        if (skill == incomingCard) {
            incomingCard = nullptr;
            exitButton.setVisible(true);
            exitButton.setActive(true);
        } else {
            hand.erase(remove(hand.begin(), hand.end(), skill));
            if (incomingCard != nullptr) {
                hand.push_back(incomingCard);
                View3DAnimation* moveRotate = new View3DAnimation(*incomingCard, 120, false, [](){}, [](){});
                moveRotate->setRotateAnimation(30, {0,0,1}, 0.2);
                moveRotate->start();
                incomingCard->addAnimation("ADJUST_ROTATE", moveRotate);
                incomingCard = nullptr;
            }
            adjustHand();
        }
    });
    moveAnim1->setMoveAnimation(skill->getPos() + (Vector3){10.0f, 10.0f, 30.0f}, 0.3);
    moveAnim1->start();
    skill->addAnimation("DISCARD", moveAnim1);
}

void SkillHandView::useCard(SkillCardView* skill) {
    disable();
    hand.erase(remove(hand.begin(), hand.end(), skill));
    usedCard = skill;
    View3DAnimation* moveAnim1 = new View3DAnimation(*skill, 120, false, [](){}, [](){});
    moveAnim1->setMoveAnimation((Vector3){skill->getPos().x, skill->getPos().y, viewCam->getPos().z} + (Vector3){2.0f, 4.0f, 0.0f}, 0.2);
    moveAnim1->start();
    skill->addAnimation("USE_1A", moveAnim1);
    View3DAnimation* rotateAnim1 = new View3DAnimation(*skill, 120, false, [](){}, [this, skill](){
        View3DAnimation* waitAnim = new View3DAnimation(*skill, 120, false, [](){}, [](){});
        waitAnim->setWait(1.5, [this, skill](){
            View3DAnimation* moveAnim2 = new View3DAnimation(*skill, 120, false, [](){}, [this, skill](){
                skill->setVisible(false);
                enable();
                adjustHand();
            });
            moveAnim2->setMoveAnimation(skill->getPos() + (Vector3){10.0f, 20.0f}, 0.4);
            moveAnim2->start();
            skill->addAnimation("USE_2", moveAnim2);
        });
        waitAnim->start();
        skill->addAnimation("USE_WAIT", waitAnim);
    });
    rotateAnim1->setRotateAnimation(-30, {0,0,1}, 0.2);
    rotateAnim1->start();
    skill->addAnimation("USE_1B", rotateAnim1);
}

string SkillHandView::catchCommand() {
    if (discardedCard != nullptr && !discarded) {
        int cardIdx = find(player.getHandCards().begin(), player.getHandCards().end(), discardedCard->getCard()) - player.getHandCards().begin();
        discarded = true;
        return to_string(cardIdx + 1);
    } else if (usedCard != nullptr && cardUsed) {
        int cardIdx = -1;
        string prefix = "GUNAKAN_KEMAMPUAN ";
        for (int i = 0; i < player.getHandCards().size(); i++) {
           if (player.getHandCards()[i] == usedCard->getCard()) {
            cardIdx = i;
           }
        }
        closeView = true;
        return prefix + to_string(cardIdx + 1);
    }
    string exitCommand = exitButton.catchCommand();
    if (exitCommand != "NULL") {
        return exitCommand;
    }
    return "NULL";
}

void SkillHandView::interactionCheck() {
    if (!active || usedCard != nullptr) return;
    if (incomingCard!= nullptr) hand.push_back(incomingCard);
    for (SkillCardView* skills : hand) {
        if (skills->isHovered(*viewCam)) {
            if (skills->getAnimation("ON_HOVER") == nullptr &&
                skills->getAnimation("START_HOVER") == nullptr &&
                skills->getAnimation("END_HOVER") == nullptr) {
                Vector3 originalPos = skills->getPos();
                View3DAnimation* animHover = new View3DAnimation(*skills, 120, false, [](){}, [this, skills, originalPos](){
                    View3DAnimation* hoverStill = new View3DAnimation(*skills, 120, false, [](){}, [](){});
                    hoverStill->setAnimationFunc([this, skills, hoverStill, originalPos](){
                        if (!skills->isHovered(*viewCam)) { 
                            hoverStill->end();
                            View3DAnimation* goBack = new View3DAnimation(*skills, 120, false, [](){}, [](){});
                            goBack->setMoveAnimation(originalPos, 0.1);
                            goBack->start();
                            skills->addAnimation("END_HOVER", goBack);
                        } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            hoverStill->end();
                            if (incomingCard != nullptr) {
                                discardCard(skills);
                            } else {
                                useCard(skills);
                            }
                        }
                    });
                    hoverStill->start();
                    skills->addAnimation("ON_HOVER", hoverStill);
                });
                if (incomingCard == skills) {
                    animHover->setMoveAnimation(skills->getPos() + (Vector3){0, 1.0f, 0}, 0.1);
                } else {
                    animHover->setMoveAnimation(skills->getPos() + (Vector3){2.0f, 1.0f, 0}, 0.1);
                }
                animHover->start();
                skills->addAnimation("START_HOVER", animHover); 
            }
        }
    }
    if (incomingCard!= nullptr) hand.erase(remove(hand.begin(), hand.end(), incomingCard));
    exitButton.interactionCheck();
}

void SkillHandView::enable() { active = true; }
void SkillHandView::disable() { active = false; }

void SkillHandView::close() {
    if (hand.size() == 0) {
        if (usedCard == nullptr) {
            closeView = true; 
        } else {
            cardUsed = true;
        }
    }
    for (SkillCardView* skillCard : hand) {
        View3DAnimation* moveBack = new View3DAnimation(*skillCard, 120, false, [](){}, [this](){ 
            if (usedCard == nullptr) {
                closeView = true; 
            } else {
                cardUsed = true;
            }
        });
        moveBack->setMoveAnimation(skillCard->getPos() - (Vector3){5.0f, 2.5f, 0}, 0.2);
        moveBack->start();
        skillCard->addAnimation("MOVE_BACK", moveBack);
    }
}

void SkillHandView::render() {
    animationCheck();
    BeginMode3D(viewCam->mount());
        for (SkillCardView* skills : hand) {
            skills->render();
        }
        if (incomingCard != nullptr) incomingCard->render();
        if (usedCard != nullptr) usedCard->render();
    EndMode3D();
    exitButton.render();
}
