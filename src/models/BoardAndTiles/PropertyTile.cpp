#include "models/BoardAndTiles/PropertyTile.hpp"
#include "models/Property/Property.hpp"

PropertyTile::PropertyTile(int index, const std::string &code, const std::string &name, TileColor color, Property *property)
    : Tile(index, code, name, color), property(property) {}

Property *PropertyTile::getProperty() const
{
    return property;
}

TileCategory PropertyTile::getCategory() const
{
    return TileCategory::PROPERTY;
}

std::string PropertyTile::getDisplayLabel() const
{
    if (property == nullptr)
        return "[" + code + "]";

    return "[" + code + "] " + Property::propertyStatusToString(property->getStatus());
}