#pragma once
#include <vector>
#include <map>
#include <string>
#include "models/BoardAndTiles/Tile.hpp"

class RailroadTile;
class JailTile;
class GoTile;

class Board
{
private:
    std::vector<Tile *> tiles;
    std::map<std::string, Tile *> tileByCode;

    int boardSize;
    GoTile *goTile = nullptr;
    JailTile *jailTile = nullptr;

public:
    Board();
    ~Board();

    Board(const Board &) = delete;
    Board &operator=(const Board &) = delete;

    void addTile(Tile *tile);

    Tile *getTile(int index) const;
    Tile *getTile(const std::string &code) const;

    int getNextIndex(int currentIndex, int steps) const;
    bool passesGo(int fromIndex, int steps) const;

    GoTile *getGoTile() const;
    JailTile *getJailTile() const;
    RailroadTile *getNearestRailroad(int currentIndex) const;

    int getSize() const;
    const std::vector<Tile *> &getAllTiles() const;

    void validate() const;
};