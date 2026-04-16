#include "models/Property/UtilityProperty.hpp"

UtilityProperty::UtilityProperty(const std::string &code,
                                 const std::string &name,
                                 int purchasePrice,
                                 int mortgageValue,
                                 const std::map<int, int> &multiplierTable)
    : Property(code, name, PropertyType::UTILITY, purchasePrice, mortgageValue),
      multiplierTable(multiplierTable)
{
}