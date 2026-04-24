#include "../include/views/viewElement/board/BoardView.hpp"
#include "../include/models/Property/StreetProperty.hpp"


void getBaseBoardTexture(RenderTexture2D* texture, int width, int height, float boardSize) {
    BeginTextureMode(*texture);
    ClearBackground({205, 230, 208, 255});
    Image img = LoadImage("data/GUIAssets/nimonspoli_logo.png");
    ImageResize(&img, img.width*1.1, img.height*1.1);
    ImageFlipHorizontal(&img);
    ImageRotate(&img, 45);
    Texture logo = LoadTextureFromImage(img);
    UnloadImage(img);
    DrawTexture(logo, (width - logo.width)/2, (height-logo.height)/2, WHITE);

    img = LoadImage("data/GUIAssets/chance_base.png");
    ImageResize(&img, img.width*0.65, img.height*0.65);
    ImageFlipHorizontal(&img);
    ImageRotate(&img, 45);
    Texture chanceBase = LoadTextureFromImage(img);
    UnloadImage(img);
    DrawTexture(chanceBase, 320, height - chanceBase.height - 320, WHITE);

    img = LoadImage("data/GUIAssets/comchest_base.png");
    ImageResize(&img, img.width*0.65, img.height*0.65);
    ImageFlipHorizontal(&img);
    ImageRotate(&img, 225);
    Texture comchestBase = LoadTextureFromImage(img);
    UnloadImage(img);
    DrawTexture(comchestBase,width - chanceBase.width - 320, 320, WHITE);
    
    

    EndTextureMode();
    UnloadTexture(logo);
    UnloadTexture(chanceBase);
    UnloadTexture(comchestBase);
}

Model getBoardModel(Board& board) {
    int tilePerDirection = board.getAllTiles().size() / 4;
    float boardSide = (tilePerDirection - 1)*TileView::getTileDim().x + 2*TileView::getTileDim().y;
    return LoadModelFromMesh(GenMeshPlane(boardSide, boardSide, 1, 1));
}

BoardView::BoardView(Board& board) : View3D({0,0.015,0}, getBoardModel(board), WHITE), board(board) {
    RenderTexture2D texture = LoadRenderTexture(1500, 1500);
    int tilePerDirection = board.getAllTiles().size() / 4;
    float boardSide = (tilePerDirection - 1)*TileView::getTileDim().x + 2*TileView::getTileDim().y;
    getBaseBoardTexture(&texture, 1500, 1500, boardSide);
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture.texture;
    int i = 0;

    Vector3 startingPos = {-(boardSide - TileView::getTileDim().y)/2, 0.03, (boardSide - TileView::getTileDim().y)/2};
    Vector3 translationNorm = {-1, 0, 0};
    Matrix tileRotation = MatrixRotate({0,1,0}, 0);
    for (Tile* tile : board.getAllTiles()) {
        TileView* tileView;
        bool isCorner = i % (tilePerDirection) == 0;
        int cardinality = i / tilePerDirection;
        if (tile->getCategory() == TileCategory::PROPERTY) {
            PropertyTile* pTile = static_cast<PropertyTile*>(tile);
            if (pTile->getProperty()->getType() == PropertyType::STREET) {
                StreetProperty* st = static_cast<StreetProperty*>(pTile->getProperty());
                tileView = new StreetTileView(*pTile, *st, isCorner, cardinality);
            }
            else {
                tileView = new PropertyTileView(*pTile, isCorner, cardinality);
            }
        } else {
            if(tile->getCode() == "PPH") {
                IncomeTaxTile* itTile = static_cast<IncomeTaxTile*>(tile);
                tileView = new TileView(*tile, "INCOME TAX", 
                                        "$" + to_string(itTile->getFlatAmount())+ " / " + to_string(itTile->getFlatAmount()) + "%",
                                        isCorner, cardinality, "data/GUIAssets/incometax_icon.png");
            } else if (tile->getCode() == "PBM") {
                LuxuryTaxTile* lTile = static_cast<LuxuryTaxTile*>(tile);
                tileView = new TileView(*tile, "LUXURY TAX", 
                                        "$" + to_string(lTile->getFlatAmount()),
                                        isCorner, cardinality, "data/GUIAssets/luxurytax_icon.png");
            } else if (tile->getCode() == "GO") {
                GoTile* gTile = static_cast<GoTile*>(tile);
                tileView = new GoTileView(*gTile, isCorner, cardinality);
            } else if (tile->getCode() == "PEN") {
                tileView = new JailTileView(*tile, isCorner, cardinality);
            } else if (tile->getCode() == "BBP") {
                tileView = new TileView(*tile, "FREE", "PARKING", isCorner, cardinality, "data/GUIAssets/freeparking_icon.png");
            } else if (tile->getCode() == "PPJ") {
                tileView = new TileView(*tile, "GO TO", "JAIL", isCorner, cardinality, "data/GUIAssets/gotojail_icon.png");
            } else if (tile->getCode() == "KSP") {
                tileView = new TileView(*tile, "CHANCE", "", isCorner, cardinality, "data/GUIAssets/chancetile_icon.png");
            } else if (tile->getCode() == "DNU") {
                tileView = new TileView(*tile, "COMMUNITY CHEST", "", isCorner, cardinality, "data/GUIAssets/comchesttile_icon.png");
            } else if (tile->getCode() == "FES") {
                tileView = new TileView(*tile, "FESTIVAL", "", isCorner, cardinality, "data/GUIAssets/festival_icon.png");
            }
        }
        tileView->movePosition(startingPos);
        

        Matrix rotation = MatrixRotate({0,1,0}, -M_PI/2);
        if (isCorner) {
            translationNorm = Vector3Transform(translationNorm, rotation);
            tileRotation = MatrixMultiply(rotation, tileRotation);
            startingPos += translationNorm*(TileView::getTileDim().x/2 + TileView::getTileDim().y/2);
        } else if ((i + 1) % tilePerDirection == 0) {
            startingPos += translationNorm*(TileView::getTileDim().x/2 + TileView::getTileDim().y/2);
        }
        else {
            startingPos += translationNorm*TileView::getTileDim().x;
        }
        i++;
        tileView->transform(tileRotation);
        tiles.push_back(tileView);
    }
    cout<<tiles.size()<<endl;
}

TileView* BoardView::getTileFromIdx(const int idx) const{
    if (idx >= tiles.size()) {
        return nullptr;
    } else {
        return tiles[idx];
    }
}

TileView* BoardView::getNextTile(const int idx) const{
    if (idx >= tiles.size()) {
        return nullptr;
    } else {
        return tiles[(idx + 1) % tiles.size()];
    }
}

TileView* BoardView::getNextTile(TileView& tile) const {
    int idx = tile.getTile()->getIndex();
    if (idx >= tiles.size()) {
        return nullptr;
    } else {
        return tiles[(idx + 1) % tiles.size()];
    }
}

TileView* BoardView::getGoTile() const {
    return getTileFromIdx(board.getGoTile()->getIndex());
}

TileView* BoardView::getJailTile() const {
    return getTileFromIdx(board.getJailTile()->getIndex());
}

const float BoardView::getBoardSize() const {
    return GetModelBoundingBox(model).max.x - GetModelBoundingBox(model).min.x;
}

void BoardView::render() {
    model.transform = transformation;
    DrawModel(model, pos, 1, color);
    int i = 0;
    for (TileView* tile : tiles) {
        tile->render();
        i++;
    }
}