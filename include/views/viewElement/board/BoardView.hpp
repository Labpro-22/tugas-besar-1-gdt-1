#pragma once
#include "../View3D.hpp"
#include "../View2D.hpp"
#include "TileView.hpp"
#include "../../../models/BoardAndTiles/Board.hpp"

class BoardView : View3D {
    private :
        Board& board;
        vector<TileView*> tiles;
    public :
        BoardView(Board& board);
        TileView* getTileFromIdx(const int idx) const;
        TileView* getGoTile() const;
        TileView* getJailTile() const;
        TileView* getNextTile(const int idx) const;
        TileView* getNextTile(TileView& tile) const;
        const float getBoardSize() const;
        void update();
        void render() override;
};