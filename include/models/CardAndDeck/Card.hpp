#ifndef CARD_HPP
#define CARD_HPP

#include <string>

enum class CardCategory {
    CHANCE,
    COMMUNITY,
    SKILL
};

class Card {
protected:
    std::string description;

public:
    Card(const std::string& description);
    virtual ~Card();

    std::string getDescription() const;
    virtual std::string getCardName() const = 0;
    virtual CardCategory getCategory() const = 0;
    virtual Card* clone() const = 0;
};

#endif