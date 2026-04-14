#include "RailroadProperty.hpp"

RailroadProperty::RailroadProperty( const std::string& code,
                                    const std::string& name,
                                    int purchasePrice,
                                    int mortgageValue,
                                    const std::map<int, int>& rentTable)
    :   Property(code, name, PropertyType::RAILROAD, purchasePrice, mortgageValue), 
        rentTable(rentTable) {
}