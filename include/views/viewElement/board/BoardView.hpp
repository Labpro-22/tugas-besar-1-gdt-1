#pragma once
#include "../View3D.hpp"
#include "../View2D.hpp"
#include "TileView.hpp"
#include "../../../models/BoardAndTiles/Board.hpp"

class BoardView : View3D {
    private :
        Board& board;
        vector<TileView> tiles;
    public :
        BoardView(Board& board);
        const float getBoardSize() const;
        void update();
        void render() override;
};