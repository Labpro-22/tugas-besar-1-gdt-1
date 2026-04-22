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



class TileView : public View3D {
    protected :
        Tile& tile;
        string tileHeader;
        string tileFooter;
        bool cornerTile;
        RenderTexture2D tileTexture;
        bool isTextureLoaded;

        static Vector2 tileDim;
    public :
        TileView(Tile& tile, const string tileHeader, const string tileFooter, const bool cornerTile, const string iconFilePath);
        static const Vector2 getTileDim();
        void render() override;
};

class PropertyTileView : public TileView {
    protected :
        Property& property;
    public :
        PropertyTileView(PropertyTile& tile, const bool cornerTile);
};

class StreetTileView : public PropertyTileView {
    private :
        StreetProperty& street;
    public :
        StreetTileView(PropertyTile& tile, StreetProperty& street, const bool cornerTile);
};

class GoTileView : public TileView {
    private:
    public:
        GoTileView(GoTile& tile, const bool cornerTile);
};

class JailTileView : public TileView {
    private:
    public:
        JailTileView(Tile& tile, const bool cornerTile);
};