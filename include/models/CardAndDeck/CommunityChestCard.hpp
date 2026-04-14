#ifndef COMMUNITY_CHEST_CARD_HPP
#define COMMUNITY_CHEST_CARD_HPP

#include "ActionCard.hpp"

enum class CommunityType {
    BIRTHDAY,
    DOCTOR_FEE,
    CAMPAIGN_FEE
};

class CommunityChestCard : public ActionCard {
private:
    CommunityType type;
    static std::string initDescription(CommunityType type);

public:
    CommunityChestCard(CommunityType type);
    ~CommunityChestCard();

    std::string getCardName() const override;
    CardCategory getCategory() const override;
    Card* clone() const override;

    CommunityType getType() const;
};

#endif