#ifndef CHANCE_CARD_HPP
#define CHANCE_CARD_HPP

#include "ActionCard.hpp"

enum class ChanceType {
    GO_TO_NEAREST_STATION,
    MOVE_BACK_3,
    GO_TO_JAIL
};

class ChanceCard : public ActionCard {
private:
    ChanceType type;
    static std::string initDescription(ChanceType type);

public:
    ChanceCard(ChanceType type);
    ~ChanceCard();

    std::string getCardName() const override;
    CardCategory getCategory() const override;
    Card* clone() const override;

    ChanceType getType() const;
};

#endif