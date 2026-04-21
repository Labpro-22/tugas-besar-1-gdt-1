#include "../include/views/viewElement/board/TileView.hpp"
#include "../include/models/BoardAndTiles/TileTypes.hpp"
#include <algorithm>
#include <ranges>

Vector2 TileView::tileDim = {2.5, 3.75};

string getSpacedString(const string s) {
    string newS = "";
    for (char c : s) {
        if (c == '_') newS += ' ';
        else newS += c;
    }
    return newS;
}

Model getTileModel(bool isCorner) {
    if (!isCorner) {
        return LoadModelFromMesh(GenMeshPlane(TileView::getTileDim().x, TileView::getTileDim().y, 1, 1));
    } else {
        return LoadModelFromMesh(GenMeshPlane(TileView::getTileDim().y, TileView::getTileDim().y, 1, 1));
    }
}

RenderTexture2D getBaseTileTexture(bool isCorner, string header, string footer, Tile& tile) {
    Color tileColor = {0,0,0,0};
    bool hasColor = true;
    switch(tile.getColor()) {
        case TileColor::COKLAT :
            tileColor = BROWN;
            break;
        case TileColor::BIRU_MUDA :
            tileColor = {0, 150, 255, 255};
            break;
        case TileColor::MERAH_MUDA :
            tileColor = PINK;
            break;
        case TileColor::BIRU_TUA :
            tileColor = BLUE;
            break;  
        case TileColor::MERAH :
            tileColor = RED;
            break;
        case TileColor::HIJAU :
            tileColor = GREEN;
            break;
        case TileColor::KUNING :
            tileColor = YELLOW;
            break;
        case TileColor::ORANYE :
            tileColor = ORANGE;
            break;
        case TileColor::ABU_ABU :
            tileColor = GRAY;
            break;
        default :
            hasColor = false;
    }



    if(!isCorner) {
        RenderTexture2D texture = LoadRenderTexture(TileView::getTileDim().x*100, TileView::getTileDim().y*100);
        BeginTextureMode(texture);
        if (hasColor) {
            DrawRectangle(0,TileView::getTileDim().y*80, TileView::getTileDim().x*100, TileView::getTileDim().y*20, tileColor);
            DrawLineEx({0, TileView::getTileDim().y*80}, {TileView::getTileDim().x*100, TileView::getTileDim().y*80},
                        6, BLACK);
        }
        DrawRectangleLinesEx({0,0, TileView::getTileDim().x*100, TileView::getTileDim().y*100}, 6, BLACK);
        EndTextureMode();

        RenderTexture2D text = LoadRenderTexture(TileView::getTileDim().x*100, TileView::getTileDim().y*100);
        BeginTextureMode(text);
        int headerSpace = 15;
        if(hasColor) {
            headerSpace += 15;
        }
        drawTextLinesWrapped(View2D::getFont("Orbitron"), header, 
                             {TileView::getTileDim().x*50, TileView::getTileDim().y*headerSpace}, 36 , 1, BLACK, TileView::getTileDim()*100);
        drawTextLinesWrapped(View2D::getFont("Orbitron"), footer, 
                             {TileView::getTileDim().x*50, TileView::getTileDim().y*90}, 36 , 1, BLACK, TileView::getTileDim()*100);
        EndTextureMode();
        
        Image textImg = LoadImageFromTexture(text.texture);
        UnloadTexture(text.texture);
        //ImageFlipVertical(&textImg);
        Texture textTexture = LoadTextureFromImage(textImg);
        UnloadImage(textImg);
        BeginTextureMode(texture);
        DrawTexture(textTexture, 0, 0, WHITE);
        EndTextureMode();
        UnloadTexture(textTexture);

        return texture;
    } else {
        RenderTexture2D texture = LoadRenderTexture(TileView::getTileDim().y*100, TileView::getTileDim().y*100);
        BeginTextureMode(texture);
        if (hasColor) {
            DrawRectangle(0,TileView::getTileDim().y*70, TileView::getTileDim().y*30, TileView::getTileDim().y*30, tileColor);
            DrawRectangleLinesEx({0,TileView::getTileDim().y*70, 
                                 TileView::getTileDim().y*30, TileView::getTileDim().y*30}, 6, BLACK);
        }
        DrawRectangleLinesEx({0,0, TileView::getTileDim().y*100, TileView::getTileDim().y*100}, 6, BLACK);
        EndTextureMode();

        Vector2 textBB = ((Vector2){TileView::TileView::getTileDim().y*0.8f, TileView::getTileDim().y*1.45f})*50*sqrt(2);
        RenderTexture2D text = LoadRenderTexture(textBB.x, textBB.y);
        BeginTextureMode(text);
        int headerSpace = 15;
        if(hasColor) {
            headerSpace += 15;
        }
        drawTextLinesWrapped(View2D::getFont("Orbitron"), header, 
                             {textBB.x/2, textBB.y*headerSpace/100}, 36 , 1, BLACK, textBB);
        drawTextLinesWrapped(View2D::getFont("Orbitron"), footer, 
                             {textBB.x/2, textBB.y*0.9f}, 36 , 1, BLACK, textBB);
        EndTextureMode();
        
        Image textImg = LoadImageFromTexture(text.texture);
        UnloadTexture(text.texture);
        ImageRotate(&textImg, 45);
        Texture textTexture = LoadTextureFromImage(textImg);
        UnloadImage(textImg);
        BeginTextureMode(texture);
        DrawTexture(textTexture, TileView::getTileDim().y*50 - textImg.width/2, TileView::getTileDim().y*50 - textImg.height/2, WHITE);
        EndTextureMode();
        UnloadTexture(textTexture);
        
        return texture;
    }
}

TileView::TileView(Tile& tile, const string tileHeader, const string tileFooter, const bool cornerTile, const string iconFilePath) :  
    View3D(View3D({0,0}, getTileModel(cornerTile), WHITE)), tile(tile), 
    tileHeader(tileHeader), tileFooter(tileFooter), cornerTile(cornerTile), 
    tileTexture(getBaseTileTexture(cornerTile, getSpacedString(tileHeader), getSpacedString(tileFooter), tile)) {
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tileTexture.texture;
}

const Vector2 TileView::getTileDim() {
    return tileDim;
}

PropertyTileView::PropertyTileView(PropertyTile& tile, const bool cornerTile) : 
    TileView(tile, tile.getProperty()->getName(), "M" + to_string(tile.getProperty()->getPurchasePrice()), 
             cornerTile, ""), property(*tile.getProperty()) {
    if (tile.getProperty()->getType() == PropertyType::RAILROAD) {
        
    } else if (tile.getProperty()->getType() == PropertyType::UTILITY) {

    }
}

StreetTileView::StreetTileView(PropertyTile& tile, StreetProperty& street, const bool cornerTile) : 
    PropertyTileView(tile, cornerTile), street(street) {}


GoTileView::GoTileView(GoTile& tile, const bool cornerTile) : 
    TileView(tile, "COLLECT M" + to_string(tile.getSalary()) + " SALARY", "", cornerTile, "data/GUIAssets/go_icon.png") {}

JailTileView::JailTileView(Tile& tile, const bool cornerTile) :
    TileView(tile, "", "JUST VISITING", cornerTile, "data/GUIAssets/jail_icon.png") {}