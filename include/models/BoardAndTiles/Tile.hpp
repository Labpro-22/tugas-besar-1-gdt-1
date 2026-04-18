#pragma once
#include <string>
#include "models/BoardAndTiles/TileTypes.hpp"

class Player;
class Game;

class Tile
{
protected:
    int index;
    std::string code;
    std::string name;
    TileColor color;

public:
    Tile(int index, const std::string &code, const std::string &name, TileColor color);
    virtual ~Tile() = default;

    int getIndex() const;
    const std::string &getCode() const;
    const std::string &getName() const;
    TileColor getColor() const;

    virtual void onLanded(Player &player, Game &game) = 0;
    virtual void onPassed(Player &player, Game &game);

    virtual TileCategory getCategory() const = 0;
    virtual std::string getDisplayLabel() const;
};