#include "models/CardAndDeck/CommunityChestCard.hpp"

CommunityChestCard::CommunityChestCard(CommunityType type)
    : ActionCard(initDescription(type)), type(type) {}

CommunityChestCard::~CommunityChestCard() {}

std::string CommunityChestCard::initDescription(CommunityType type) {
    switch (type) {
        case CommunityType::BIRTHDAY:
            return "Ini adalah hari ulang tahun Anda. Dapatkan M100 dari setiap pemain.";
        case CommunityType::DOCTOR_FEE:
            return "Biaya dokter. Bayar M700.";
        case CommunityType::CAMPAIGN_FEE:
            return "Anda mau nyaleg. Bayar M200 kepada setiap pemain.";
        default:
            return "";
    }
}

std::string CommunityChestCard::getCardName() const {
    return "CommunityChestCard";
}

CardCategory CommunityChestCard::getCategory() const {
    return CardCategory::COMMUNITY;
}

CommunityType CommunityChestCard::getType() const {
    return type;
}

Card* CommunityChestCard::clone() const {
    return new CommunityChestCard(type);
}