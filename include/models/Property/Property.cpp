#include "Property.hpp"

Property::Property( const std::string& code,
                    const std::string& name,
                    PropertyType type,
                    int purchasePrice,
                    int mortgageValue)
    :   code(code),
        name(name),
        type(type),
        purchasePrice(purchasePrice),
        mortgageValue(mortgageValue),
        owner(nullptr),
        status(PropertyStatus::BANK),
        festivalMultiplier(1),
        festivalDuration(0) {
}