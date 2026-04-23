#pragma once
#include "models/BoardAndTiles/ActionTile.hpp"

class TaxTile : public ActionTile
{
public:
    TaxTile(int index, const std::string &code, const std::string &name);

    void onLanded(Player &player, Game &game) override;
    virtual int calculateTax(const Player &player) const = 0;
};