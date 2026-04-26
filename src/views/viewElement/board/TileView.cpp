#include "../include/views/viewElement/board/TileView.hpp"
#include "../include/views/viewElement/player/PlayerView.hpp"
#include "../include/views/animation/ViewAnimation.hpp"
#include "../include/models/BoardAndTiles/TileTypes.hpp"
#include <algorithm>
#include <ranges>

Vector2 TileView::tileDim = {2.5, 3.75};
Model* StreetTileView::houseModel = nullptr;

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

TileView::TileView(Tile& tile, const string tileHeader, const string tileFooter, const bool cornerTile, const int cardinality, const string iconFilePath) :  
    View3D(View3D({0,0}, getTileModel(cornerTile), WHITE)), tile(tile), isTextureLoaded(false),
    tileHeader(tileHeader), tileFooter(tileFooter), cornerTile(cornerTile), cardinality(cardinality),
    tileTexture(getBaseTileTexture(cornerTile, getSpacedString(tileHeader), getSpacedString(tileFooter), tile)) {
    if (iconFilePath != "") {
        drawImageOnTexture(&tileTexture, iconFilePath, cornerTile);
    }
}

Tile* TileView::getTile() const {
    return &tile;
}

int TileView::getCardinality() const {
    return cardinality;
}

bool TileView::isCornerTile() const {
    return cornerTile;
}

void TileView::setCamToTile(View3DCamera* cam) const {
    cam->moveTargetPos(pos + (Vector3){0,1.0f,0});
    cam->movePosition(pos + Vector3Transform({0,2.0f,-5.0f}, MatrixRotate({0,1,0}, (-cardinality + 1)*M_PI/2)));
}

void TileView::render() {
    if (!isTextureLoaded) {
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tileTexture.texture;
        isTextureLoaded = true;
    }
    model.transform = transformation;
    DrawModel(model, pos, 1, color);
}

void StreetTileView::render() {
    animationCheck();
    if (!isTextureLoaded) {
        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tileTexture.texture;
        isTextureLoaded = true;
    }
    model.transform = transformation;
    DrawModel(model, pos, 1, color);
    if (houses.size() <= 4) {
        for (View3D* house : houses) {
            house->render();
        }
    } else {
        houses.at(4)->render();
    }
}

void StreetTileView::loadHouseModel(string filepath) {
    houseModel = new Model (LoadModel(filepath.c_str()));
}


Vector3 TileView::getPlayerPosInTile(int playerIdx) {
    Vector2 tileSize = tileDim;
    if (cornerTile) {
        tileSize.x = tileDim.y;
    } else {
        tileSize.x *= 0.85;
    }
    Vector3 relPlayerPos;
    if (playersInTile.size() == 1) {
        relPlayerPos =  {0, 0, 0};
    } else if (playersInTile.size() == 2) {
        if (playerIdx == 0) relPlayerPos = {0, 0, tileSize.y*0.325f};
        else relPlayerPos = {0, 0, -tileSize.y*0.325f};
    } else {
        if (playerIdx == 0) relPlayerPos = {-tileSize.x*0.325f, 0, tileSize.y*0.325f};
        else if (playerIdx == 1) relPlayerPos = {-tileSize.x*0.325f, 0, -tileSize.y*0.325f};
        else if (playerIdx == 2) relPlayerPos = {tileSize.x*0.325f, 0, -tileSize.y*0.325f};
        else relPlayerPos = {tileSize.x*0.325f, 0, tileSize.y*0.325f};
    }
    return pos + Vector3Transform(relPlayerPos, MatrixRotate({0,1,0}, (-cardinality + 1)*M_PI/2));
}

Vector3 StreetTileView::getPlayerPosInTile(int playerIdx) {
    Vector2 tileSize = tileDim;
    if (cornerTile) {
        tileSize.x = tileDim.y;
    } else {
        tileSize.x *= 0.85;
    }
    Vector3 relPlayerPos;
    if (!cornerTile) {
        if (playersInTile.size() == 1) {
            relPlayerPos =  {0, 0, 0};
        } else if (playersInTile.size() == 2) {
            if (playerIdx == 0) relPlayerPos = {0, 0, tileSize.y*0.175f};
            else relPlayerPos = {0, 0, -tileSize.y*0.325f};
        } else {
            if (playerIdx == 0) relPlayerPos = {-tileSize.x*0.325f, 0, tileSize.y*0.175f};
            else if (playerIdx == 1) relPlayerPos = {-tileSize.x*0.325f, 0, -tileSize.y*0.325f};
            else if (playerIdx == 2) relPlayerPos = {tileSize.x*0.325f, 0, -tileSize.y*0.325f};
            else {relPlayerPos = {tileSize.x*0.325f, 0, tileSize.y*0.175f};}
            
        }
        return pos + Vector3Transform(relPlayerPos, MatrixRotate({0,1,0}, (-cardinality + 1)*M_PI/2));
    } else {
        if (playersInTile.size() == 1) {
            relPlayerPos =  {tileSize.x*0.175f, 0, tileSize.y*0.175f};
        } else if (playersInTile.size() == 2) {
            if (playerIdx == 0) relPlayerPos = {0, 0, tileSize.y*0.25f};
            else relPlayerPos = {tileSize.x*0.25f, 0, 0};
        } else {
            if (playerIdx == 0) relPlayerPos = {-tileSize.x*0.325f, 0, tileSize.y*0.325f};
            else if (playerIdx == 1) relPlayerPos = {tileSize.x*0.325f, 0, tileSize.y*0.325f};
            else if (playerIdx == 2) relPlayerPos = {tileSize.x*0.325f, 0, -tileSize.y*0.325f};
            else relPlayerPos = {tileSize.x*0.175f, 0, -tileSize.y*0.175f};            
        }
        return pos + Vector3Transform(relPlayerPos, MatrixRotate({0,1,0}, (cardinality + 1)*M_PI/2));
    }
}

Vector3 JailTileView::getPlayerPosInTile(int playerIdx) {
    Vector2 tileSize = tileDim;
    if (cornerTile) {
        tileSize.x = tileDim.y;
    } else {
        tileSize.x *= 0.85;
    }
    int jailIdx = find(jailedPlayers.begin(), jailedPlayers.end(), playersInTile[playerIdx]) - jailedPlayers.begin();
    if (jailIdx < jailedPlayers.size()) {
        Vector3 relPlayerPos;
        if (cornerTile) {
            if (jailedPlayers.size() == 1) {
                relPlayerPos = {-tileSize.x*0.175f, 0, -tileSize.y*0.175f};
            } else if (jailedPlayers.size() == 2) {
                if (jailIdx == 0) relPlayerPos = {-tileSize.x*0.325f, 0, -tileSize.y*0.325f};
                else relPlayerPos = {-tileSize.x*0.025f, 0, -tileSize.y*0.025f};
            } else {
                if (jailIdx == 0) relPlayerPos = {-tileSize.x*0.325f, 0, -tileSize.y*0.325f};
                else if (jailIdx == 1) relPlayerPos = {-tileSize.x*0.025f, 0, -tileSize.y*0.325f};
                else if (jailIdx == 2) relPlayerPos = {-tileSize.x*0.325f, 0, -tileSize.y*0.025f};
                else relPlayerPos = {-tileSize.x*0.025f, 0, -tileSize.y*0.025f};
            }
            return pos + Vector3Transform(relPlayerPos, MatrixRotate({0,1,0}, (cardinality + 1)*M_PI/2));
        }
        else {
            if (playersInTile.size() == 1) {
                relPlayerPos =  {0, 0, tileSize.y*0.175f};
            } else if (playersInTile.size() == 2) {
                if (jailIdx == 0) relPlayerPos = {tileSize.x*0.325f, 0, tileSize.y*0.175f};
                else relPlayerPos = {-tileSize.x*0.325f, 0, tileSize.y*0.175f};
            } else {
                if (jailIdx == 0) relPlayerPos = {tileSize.x*0.325f, 0, tileSize.y*0.025f};
                else if (jailIdx == 1) relPlayerPos = {-tileSize.x*0.325f, 0, tileSize.y*0.025f};
                else if (jailIdx == 2) relPlayerPos = {tileSize.x*0.325f, 0, tileSize.y*0.325f};
                else relPlayerPos = {-tileSize.x*0.325f, 0, tileSize.y*0.325f};
            }
            return pos + Vector3Transform(relPlayerPos, MatrixRotate({0,1,0}, (-cardinality + 1)*M_PI/2));
        }
    }
    int visitingIdx = playerIdx;
    for (int i = 0; i < playerIdx; i++) {
        if (find(jailedPlayers.begin(), jailedPlayers.end(), playersInTile[playerIdx]) != jailedPlayers.end()) {
            visitingIdx--;
        }
    }
    Vector3 relPlayerPos;
    if(cornerTile) {
        if (playersInTile.size() == 1) {
            relPlayerPos =  {tileSize.x*0.325f, 0, tileSize.y*0.325f};
        } else if (playersInTile.size() == 2) {
            if (visitingIdx == 0) relPlayerPos = {0, 0, tileSize.y*0.325f};
            else relPlayerPos = {tileSize.x*0.325f, 0, 0};
        } else if (playersInTile.size() == 3) {
            if (visitingIdx == 0) relPlayerPos = {-tileSize.x*0.325f, 0, tileSize.y*0.325f};
            else if (visitingIdx == 1) relPlayerPos = {tileSize.x*0.325f, 0, tileSize.y*0.325f};
            else relPlayerPos = {tileSize.x*0.325f, 0, -tileSize.y*0.325f};
        } else {
            if (visitingIdx == 0) relPlayerPos = {-tileSize.x*0.325f, 0, tileSize.y*0.325f};
            else if (visitingIdx == 1) relPlayerPos = {tileSize.x*0.075f, 0, tileSize.y*0.325f};
            else if (visitingIdx == 2) relPlayerPos = {tileSize.x*0.325f, 0, tileSize.y*0.075f};
            else relPlayerPos = {tileSize.x*0.325f, 0, -tileSize.y*0.325f};
        }
        return pos + Vector3Transform(relPlayerPos, MatrixRotate({0,1,0}, (cardinality + 1)*M_PI/2));
    } else {
        if (playersInTile.size() == 1) {
            relPlayerPos =  {0, 0, -tileSize.y*0.125f};
        } else if (playersInTile.size() == 2) {
            if (visitingIdx == 0) relPlayerPos = {tileSize.x*0.325f, 0, -tileSize.y*0.15f};
            else relPlayerPos = {-tileSize.x*0.325f, 0, -tileSize.y*0.15f};
        } else {
            if (visitingIdx == 0) relPlayerPos = {tileSize.x*0.325f, 0, -tileSize.y*0.125f};
            else if (visitingIdx == 1) relPlayerPos = {-tileSize.x*0.325f, 0, -tileSize.y*0.125f};
            else if (visitingIdx == 2) relPlayerPos = {tileSize.x*0.325f, 0, -tileSize.y*0.325f};
            else relPlayerPos = {-tileSize.x*0.325f, 0, -tileSize.y*0.325f};
        }
        return pos + Vector3Transform(relPlayerPos, MatrixRotate({0,1,0}, (-cardinality + 1)*M_PI/2));
    }
}

Vector3 TileView::getPassingPos() {
    if (playersInTile.size() == 1) {
        return pos + Vector3Transform((Vector3){0,0,-tileDim.y*0.25f}, MatrixRotate({0,1,0}, (-cardinality + 1)*M_PI/2));
    } else {
        return pos;
    }
}

Vector3 StreetTileView::getPassingPos() {
    if (playersInTile.size() > 1) {
        return pos + Vector3Transform((Vector3){0,0,-tileDim.y*0.125f}, MatrixRotate({0,1,0}, (-cardinality + 1)*M_PI/2));
    } else if (playersInTile.size() == 1){
        return pos + Vector3Transform((Vector3){0,0,-tileDim.y*0.25f}, MatrixRotate({0,1,0}, (-cardinality + 1)*M_PI/2));
    } else {
        return pos;
    }
}

Vector3 JailTileView::getPassingPos() {
    return pos + Vector3Transform((Vector3){0, 0, -tileDim.y*0.325f}, MatrixRotate({0,1,0}, (-cardinality + 1)*M_PI/2));
}

void TileView::adjustPlayersInTile() {
    for (int i = 0; i < playersInTile.size(); i++) {
        View3DAnimation* adjustAnim = new View3DAnimation(*playersInTile[i], 120, true, [](){}, [](){});
        adjustAnim->setMoveAnimation(getPlayerPosInTile(i), 0.1);
        playersInTile[i]->addAnimation("ADJUST_POS", adjustAnim);
        playersInTile[i]->getAnimation("ADJUST_POS")->start();
    }
}   

void TileView::handlePlayerEnteringTile(PlayerView* player) {
    playersInTile.push_back(player);
    adjustPlayersInTile();
}

void TileView::handlePlayerLeavingTile(PlayerView* player) {
    playersInTile.erase(remove(playersInTile.begin(), playersInTile.end(), player));
    adjustPlayersInTile();
}

const Vector2 TileView::getTileDim() {
    return tileDim;
}

PropertyTileView::PropertyTileView(PropertyTile& tile, const bool cornerTile,const int cardinality) : 
    TileView(tile, tile.getProperty()->getName(), "M" + to_string(tile.getProperty()->getPurchasePrice()),
             cornerTile, cardinality, ""), property(*tile.getProperty()) {
    if (tile.getProperty()->getType() == PropertyType::RAILROAD) {
        drawImageOnTexture(&tileTexture, "data/GUIAssets/railroad_icon.png", cornerTile);
    } else if (tile.getProperty()->getType() == PropertyType::UTILITY) {
        drawImageOnTexture(&tileTexture, "data/GUIAssets/utility"+ tile.getCode() + "_icon.png", cornerTile);
    }
}

StreetTileView::StreetTileView(PropertyTile& tile, StreetProperty& street, const bool cornerTile,const int cardinality) : 
    PropertyTileView(tile, cornerTile, cardinality), street(street) {}


void StreetTileView::buildHouse() {
    Color buildingColor = DARKGREEN;
    if (houses.size() >= 4) {
        buildingColor = RED;
    }
    houses.push_back(new View3D(pos, LoadModel("data/GUIAssets/house_model.obj"), buildingColor));
    BoundingBox modelBB = GetModelBoundingBox(houses.back()->getModel());
    float scale = (tileDim.y*0.2f)/(modelBB.max.x - modelBB.min.x);
    float scaleHotel = 1;
    if (houses.size() >= 5) {
        scaleHotel = 1.5f;
    }
    houses.back()->setTransform(MatrixScale(scale, scale*scaleHotel, scale*scaleHotel)*MatrixRotate({0,1,0}, (cardinality)*M_PI/2));
    houses.back()->setPosY(((modelBB.max.y - modelBB.min.y)/2 + 0.2)*scale*scaleHotel);
    Vector3 moveTranslation;
    if (!cornerTile) {
        float xTrans = 0;
        if (houses.size() < 5) {
            xTrans = -tileDim.x*0.375 + (houses.size() - 1)*tileDim.x*0.25;
        }
        moveTranslation = Vector3Transform({-xTrans, 0, tileDim.y/2 - (modelBB.max.x - modelBB.min.x)*scale*0.5f}, MatrixRotate({0,1,0}, (-cardinality + 1)*M_PI/2));
    }
    else {
        float xTrans = 0;
        Vector3 baseTranslation = {tileDim.y/2 - (modelBB.max.x - modelBB.min.x)*scale*0.5f, 0,
                                   tileDim.y/2 - (modelBB.max.x - modelBB.min.x)*scale*0.5f};
        if (houses.size() == 2) {
            baseTranslation -= Vector3{tileDim.y*0.2f, 0 ,0};
        } else if (houses.size() == 3) {
            baseTranslation -= Vector3{0, 0 , tileDim.y*0.2f};
        } else if (houses.size() == 4) {
            baseTranslation -= Vector3{tileDim.y*0.2f, 0 , tileDim.y*0.2f};
        } else if (houses.size() == 5) {
            baseTranslation -= Vector3{tileDim.y*0.1f, 0 , tileDim.y*0.1f};
        }
        moveTranslation = Vector3Transform(baseTranslation, MatrixRotate({0,1,0}, (-cardinality + 1)*M_PI/2));
    }
    houses.back()->movePositionDelta(moveTranslation);


    Vector3 originalPos = houses.back()->getPos();
    houses.back()->setPosY(-originalPos.y);
    View3DAnimation* riseAnim = new View3DAnimation(*houses.back(), 120, false, [](){}, [](){});
    riseAnim->setMoveAnimation(originalPos, 0.4);
    riseAnim->start();
    houses.back()->addAnimation("RISE", riseAnim);
}

void StreetTileView::sellHouse() {
    View3D* soldHouse = houses.back();
    Vector3 destPos = {soldHouse->getPos().x, -soldHouse->getPos().y, soldHouse->getPos().z};
    View3DAnimation* fallAnim = new View3DAnimation(*soldHouse, 120, false, [](){}, [soldHouse, this](){
        houses.pop_back();
        delete soldHouse;
    });
    fallAnim->setMoveAnimation(destPos, 0.4);
    fallAnim->start();
    addAnimation("FALL", fallAnim);
}

GoTileView::GoTileView(GoTile& tile, const bool cornerTile, const int cardinality) : 
    TileView(tile, "COLLECT M" + to_string(tile.getSalary()) + " SALARY", "", cornerTile, cardinality, "data/GUIAssets/go_icon.png") {
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



JailTileView::JailTileView(Tile& tile, const bool cornerTile, const int cardinality) :
    TileView(tile, "IN JAIL", "JUST VISITING", cornerTile, cardinality, "") {
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

void JailTileView::handlePlayerEnteringJail(PlayerView* player) {
    jailedPlayers.push_back(player);
}


void JailTileView::handlePlayerLeavingTile(PlayerView* player) {
    playersInTile.erase(remove(playersInTile.begin(), playersInTile.end(), player));
    jailedPlayers.erase(remove(jailedPlayers.begin(), jailedPlayers.end(), player));
    adjustPlayersInTile();
}