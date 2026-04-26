#include "CardView.hpp"
#include "../Popup.hpp"
#include "../../../models/Player/Player.hpp"
#include "../../animation/camera/View3DCamera.hpp"


class SkillHandView : public IndefinitePopup {
    private:
        Player& player;
        vector<SkillCardView*> hand;
        SkillCardView* incomingCard;
        SkillCardView* usedCard;
        SkillCardView* discardedCard;
        View3DCamera* viewCam;
        bool active;
        bool discarded;
        bool cardUsed;
    public:
        SkillHandView(Player& player, View3DCamera* viewCam, Card* incomingCard);
        ~SkillHandView() override;
        void enable() override;
        void disable() override;
        void discardCard(SkillCardView* skill);
        void adjustHand();
        void useCard(SkillCardView* skill);
        string catchCommand() override;
        void close() override;
        void interactionCheck() override;
        void render() override;
};