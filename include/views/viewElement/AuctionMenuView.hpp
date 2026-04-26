#include "MenuView.hpp"
#include "../../core/AuctionManager.hpp"
#include "../../core/Game.hpp"
#include "Entry.hpp"
#include "player/PlayerProfileView.hpp"


class AuctionMenuView : public MenuView {
    private:
        Property* auctionedProperty;
        vector<Player*> players;
        Player* auctioner;
        Player* highestBidder;
        Player* currentBidder;
        int bidAmount;
        Entry bidEntry;
        Interactable passButton;
        Interactable bidButton;
        vector<PlayerProfileView *> playerProfiles;
    public:
        AuctionMenuView(Property* auctionedProperty, Game* game, Player* auctioner,vector<PlayerProfileView *> playerProfiles);
        void initiateAuctionTurn(Player* currentPlayer, bool forceBid);
        void updateAuction(Player* highestBidder, int highestBid);
        void endAuction(Player* winner);
        string catchCommand() override;
        void interactionCheck() override;
        void render() override;
};