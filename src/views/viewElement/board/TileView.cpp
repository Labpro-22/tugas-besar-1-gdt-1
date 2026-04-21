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
        ClearBackground({205, 230, 208, 255});
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
        drawTextLinesWrapped(View2D::getFont("Kabel"), header, 
                             {TileView::getTileDim().x*50, TileView::getTileDim().y*headerSpace}, 32 , 1, BLACK, TileView::getTileDim()*100);
        drawTextLinesWrapped(View2D::getFont("Kabel"), footer, 
                             {TileView::getTileDim().x*50, TileView::getTileDim().y*90}, 32 , 1, BLACK, TileView::getTileDim()*100);
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
        ClearBackground({205, 230, 208, 255});
        if (hasColor) {
            DrawRectangle(0,TileView::getTileDim().y*60, TileView::getTileDim().y*40, TileView::getTileDim().y*40, tileColor);
            DrawRectangleLinesEx({0,TileView::getTileDim().y*60, 
                                 TileView::getTileDim().y*40, TileView::getTileDim().y*40}, 6, BLACK);
        }
        DrawRectangleLinesEx({0,0, TileView::getTileDim().y*100, TileView::getTileDim().y*100}, 6, BLACK);
        EndTextureMode();

        Vector2 textBB = ((Vector2){TileView::TileView::getTileDim().y*0.8f, TileView::getTileDim().y*1.45f})*50*sqrt(2);
        RenderTexture2D text = LoadRenderTexture(textBB.x, textBB.y);
        BeginTextureMode(text);
        int headerSpace = 15;
        if(hasColor) {
            headerSpace += 30;
        }
        drawTextLinesWrapped(View2D::getFont("Kabel"), header, 
                             {textBB.x/2, textBB.y*headerSpace/100}, 32 , 1, BLACK, textBB);
        drawTextLinesWrapped(View2D::getFont("Kabel"), footer, 
                             {textBB.x/2, textBB.y*0.9f}, 32 , 1, BLACK, textBB);
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

void drawImageOnTexture(RenderTexture2D* texture, string filePath, bool isCorner) {
    Image img = LoadImage(filePath.c_str());
    ImageFlipVertical(&img);
    if (img.width > texture->texture.width*0.6) {
        ImageResize(&img, texture->texture.width*0.6, img.height*(texture->texture.width*0.6/img.width));
    } 
    if (img.height > texture->texture.height*0.5) {
        ImageResize(&img, img.width*(texture->texture.height*0.5/img.height), texture->texture.height*0.5);
    }
    if (isCorner) {
        ImageRotate(&img, 45);
    }
    Texture imgTexture = LoadTextureFromImage(img);
    
    BeginTextureMode(*texture);
    DrawTexture(imgTexture, (texture->texture.width - img.width)/2, (texture->texture.height - img.height)/2, WHITE);
    EndTextureMode();
    UnloadImage(img);
    UnloadTexture(imgTexture);
}

TileView::TileView(Tile& tile, const string tileHeader, const string tileFooter, const bool cornerTile, const string iconFilePath) :  
    View3D(View3D({0,0}, getTileModel(cornerTile), WHITE)), tile(tile), isTextureLoaded(false),
    tileHeader(tileHeader), tileFooter(tileFooter), cornerTile(cornerTile), 
    tileTexture(getBaseTileTexture(cornerTile, getSpacedString(tileHeader), getSpacedString(tileFooter), tile)) {
    if (iconFilePath != "") {
        drawImageOnTexture(&tileTexture, iconFilePath, cornerTile);
    }
}

void TileView::render() {
    if (!isTextureLoaded) {
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tileTexture.texture;
        isTextureLoaded = true;
    }
    model.transform = transformation;
    DrawModel(model, pos, 1, color);
}

const Vector2 TileView::getTileDim() {
    return tileDim;
}

PropertyTileView::PropertyTileView(PropertyTile& tile, const bool cornerTile) : 
    TileView(tile, tile.getProperty()->getName(), "M" + to_string(tile.getProperty()->getPurchasePrice()), 
             cornerTile, ""), property(*tile.getProperty()) {
    if (tile.getProperty()->getType() == PropertyType::RAILROAD) {
        drawImageOnTexture(&tileTexture, "data/GUIAssets/railroad_icon.png", cornerTile);
    } else if (tile.getProperty()->getType() == PropertyType::UTILITY) {
        drawImageOnTexture(&tileTexture, "data/GUIAssets/utility"+ tile.getCode() + "_icon.png", cornerTile);
    }
}

StreetTileView::StreetTileView(PropertyTile& tile, StreetProperty& street, const bool cornerTile) : 
    PropertyTileView(tile, cornerTile), street(street) {}


GoTileView::GoTileView(GoTile& tile, const bool cornerTile) : 
    TileView(tile, "COLLECT M" + to_string(tile.getSalary()) + " SALARY", "", cornerTile, "data/GUIAssets/go_icon.png") {
    BeginTextureMode(tileTexture);
    Image img = LoadImage("data/GUIAssets/goarrow_icon.png");
    ImageFlipVertical(&img);
    if (img.width > tileTexture.texture.width*0.6) {
        ImageResize(&img, tileTexture.texture.width*0.6, img.height*(tileTexture.texture.width*0.6/img.width));
    }
    Texture2D imgTexture = LoadTextureFromImage(img);
    
    DrawTexture(imgTexture, (tileTexture.texture.width - img.width)/2, tileTexture.texture.height*0.02, WHITE);
    EndDrawing();
    UnloadImage(img);
    UnloadTexture(imgTexture);
}

JailTileView::JailTileView(Tile& tile, const bool cornerTile) :
    TileView(tile, "IN JAIL", "JUST VISITING", cornerTile, "") {
    BeginTextureMode(tileTexture);
    ClearBackground({205, 230, 208, 255});
    if(!cornerTile) {
        DrawRectangle(0,TileView::getTileDim().y*40, TileView::getTileDim().x*100, TileView::getTileDim().y*60, ORANGE);
        DrawRectangleLinesEx({0,TileView::getTileDim().y*40, TileView::getTileDim().x*100, TileView::getTileDim().y*60},6, BLACK);
        DrawRectangleLinesEx({0,0,TileView::getTileDim().y*100, TileView::getTileDim().y*100}, 6, BLACK);
        EndTextureMode();

        RenderTexture2D text = LoadRenderTexture(TileView::getTileDim().x*100, TileView::getTileDim().y*100);
        BeginTextureMode(text);

        drawTextLinesWrapped(View2D::getFont("Kabel"), tileHeader, 
                             {TileView::getTileDim().x*50, TileView::getTileDim().y*10}, 32 , 1, BLACK, TileView::getTileDim()*100);
        drawTextLinesWrapped(View2D::getFont("Kabel"), tileFooter, 
                             {TileView::getTileDim().x*50, TileView::getTileDim().y*90}, 32 , 1, BLACK, TileView::getTileDim()*80);
        EndTextureMode();
        
        Image textImg = LoadImageFromTexture(text.texture);
        UnloadTexture(text.texture);
        //ImageFlipVertical(&textImg);
        Texture textTexture = LoadTextureFromImage(textImg);
        Image icon = LoadImage("data/GUIAssets/jail_icon.png");
        if (icon.height > getTileDim().y*35) {
            ImageResize(&icon, icon.width*(getTileDim().y*35/icon.height), getTileDim().y*35);
        }
        Texture iconTexture = LoadTextureFromImage(icon);
        UnloadImage(icon);
        UnloadImage(textImg);
        BeginTextureMode(tileTexture);
        DrawTexture(textTexture, 0, 0, WHITE);
        DrawTexture(iconTexture, (getTileDim().x*100 - iconTexture.width)/2, (getTileDim().y*60 + iconTexture.width)/2, WHITE);
        EndTextureMode();
        UnloadTexture(iconTexture);
        UnloadTexture(textTexture);
    } else {
        DrawRectangle(0,getTileDim().y*35, getTileDim().y*65, getTileDim().y*65, ORANGE);
        DrawRectangleLinesEx({0,getTileDim().y*35, getTileDim().y*65, getTileDim().y*65},6, BLACK);
        DrawRectangleLinesEx({0,0,getTileDim().y*100, getTileDim().y*100}, 6, BLACK);
        EndTextureMode();
        Vector2 textBB = ((Vector2){getTileDim().y*0.8f, getTileDim().y*1.45f})*32.5f*sqrt(2);
        RenderTexture2D text = LoadRenderTexture(textBB.x, textBB.y);
        BeginTextureMode(text);
        float headerSpace = 0.15;
        drawTextLinesWrapped(View2D::getFont("Kabel"), "IN", 
                             {textBB.x/2, textBB.y*headerSpace}, 32 , 1, BLACK, textBB);
        drawTextLinesWrapped(View2D::getFont("Kabel"), "JAIL", 
                             {textBB.x/2, textBB.y*0.9f}, 32 , 1, BLACK, textBB);
        EndTextureMode();
        Image textImg = LoadImageFromTexture(text.texture);
        UnloadTexture(text.texture);
        ImageRotate(&textImg, 45);
        Texture textTexture = LoadTextureFromImage(textImg);
        UnloadImage(textImg);
        BeginTextureMode(tileTexture);
        DrawTexture(textTexture, getTileDim().y*32.5 - textImg.width/2, 
                    getTileDim().y*67.5 - textImg.height/2, WHITE);
        

        Image icon = LoadImage("data/GUIAssets/jail_icon.png");
        ImageFlipVertical(&icon);
        if (icon.width > textBB.x*0.9) {
            ImageResize(&icon, textBB.x*0.9, icon.height*(textBB.x*0.9/icon.width));
        } 
        ImageRotate(&icon, 45);
        
        Texture iconTexture = LoadTextureFromImage(icon);
        DrawTexture(iconTexture,getTileDim().y*32.5 - icon.width/2, 
                    getTileDim().y*67.5 - icon.height/2, WHITE);
        EndTextureMode();

        RenderTexture2D textTexture1 = LoadRenderTexture(getTileDim().y*100, getTileDim().y*100);
        Vector2 textDim = MeasureTextEx(View2D::getFont("Kabel"), "JUST", 44, 1);
        BeginTextureMode(textTexture1);
        drawTextLinesWrapped(View2D::getFont("Kabel"), "JUST", 
                             {getTileDim().y*40, getTileDim().y*100 - textDim.y}, 44 , 1, BLACK, {getTileDim().y*100, getTileDim().y*100});
        EndTextureMode();
        Image textImg1 = LoadImageFromTexture(textTexture1.texture);
        UnloadTexture(textTexture1.texture);
        ImageFlipVertical(&textImg1);
        ImageFlipHorizontal(&textImg1);
        ImageRotate(&textImg1, 180);
        Texture textTexture1b = LoadTextureFromImage(textImg1);

        RenderTexture2D textTexture2 = LoadRenderTexture(getTileDim().y*100, getTileDim().y*100);
        textDim = MeasureTextEx(View2D::getFont("Kabel"), "VISITING", 44, 1);
        BeginTextureMode(textTexture2);
        drawTextLinesWrapped(View2D::getFont("Kabel"), "VISITING", 
                             {getTileDim().y*60, getTileDim().y*100 - textDim.y}, 44 , 1, BLACK, {getTileDim().y*100, getTileDim().y*100});
        EndTextureMode();
        Image textImg2 = LoadImageFromTexture(textTexture2.texture);
        UnloadTexture(textTexture2.texture);
        ImageFlipVertical(&textImg2);
        ImageFlipHorizontal(&textImg2);
        ImageRotate(&textImg2, 270);
        Texture textTexture2b = LoadTextureFromImage(textImg2);

        BeginTextureMode(tileTexture);
        DrawTexture(textTexture1b, 0, 0, WHITE);
        DrawTexture(textTexture2b, 0, 0, WHITE);

        EndTextureMode();
        UnloadImage(icon);
        UnloadImage(textImg1);
        UnloadImage(textImg2);
        UnloadTexture(iconTexture);
        UnloadTexture(textTexture);
        UnloadTexture(textTexture1b);
        UnloadTexture(textTexture2b);
    }
}