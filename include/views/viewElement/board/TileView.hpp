#pragma once
#include "../View2D.hpp"
#include "../View3D.hpp"
#include "../../../models/BoardAndTiles/Tile.hpp"
#include "../../../models/BoardAndTiles/TileTypes.hpp"
#include "../../../models/BoardAndTiles/PropertyTile.hpp"
#include "../../../models/BoardAndTiles/PropertyTile/StreetTile.hpp"
#include "../../../models/BoardAndTiles/ActionTile/TaxTile/IncomingTaxTile.hpp"
#include "../../../models/BoardAndTiles/ActionTile/TaxTile/LuxuryTaxTile.hpp"
#include "../../../models/BoardAndTiles/SpecialTile/GoTile.hpp"
#include "../../../models/BoardAndTiles/SpecialTile/JailTile.hpp"
#include "../../../models/Property/Property.hpp"
#include "../../../models/Property/StreetProperty.hpp"

class PlayerView;

class TileView : public View3D {
    protected :
        Tile& tile;
        string tileHeader;
        string tileFooter;
        int cardinality;
        bool cornerTile;
        RenderTexture2D tileTexture;
        bool isTextureLoaded;
        vector<PlayerView*> playersInTile;

        static Vector2 tileDim;
    public :
        TileView(Tile& tile, const string tileHeader, const string tileFooter, const bool cornerTile, const int cardinality, const string iconFilePath);
        Tile* getTile() const;
        int getCardinality() const;
        bool isCornerTile() const;
        virtual Vector3 getPlayerPosInTile(int playerIdx);
        virtual Vector3 getPassingPos();
        void adjustPlayersInTile();
        void handlePlayerEnteringTile(PlayerView* player);
        static const Vector2 getTileDim();
        void render() override;
};

class PropertyTileView : public TileView {
    protected :
        Property& property;
    public :
        PropertyTileView(PropertyTile& tile, const bool cornerTile, const int cardinality);
};

class StreetTileView : public PropertyTileView {
    private :
        StreetProperty& street;
    public :
        StreetTileView(PropertyTile& tile, StreetProperty& street, const bool cornerTile, const int cardinality);
        Vector3 getPlayerPosInTile(int playerIdx) override;
        Vector3 getPassingPos() override;
};

class GoTileView : public TileView {
    private:
    public:
        GoTileView(GoTile& tile, const bool cornerTile, const int cardinality);
};

class JailTileView : public TileView {
    private:
    public:
        JailTileView(Tile& tile, const bool cornerTile, const int cardinality);
        Vector3 getPlayerPosInTile(int playerIdx) override;
        Vector3 getPassingPos() override;
};