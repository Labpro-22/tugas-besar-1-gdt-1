#include "../include/views/viewElement/cards/CardView.hpp"
#include "../include/views/viewElement/cards/CardPileView.hpp"

Model createCardModel(Vector2 cardDim) {
    Mesh cardMesh = GenMeshCube(cardDim.x, 0.01f, cardDim.y);
    int i = 3;
    int j = 2;
    float x = 0.5;
    vector<float> Yloc = {0.25, 0.75};
    cardMesh.texcoords[i*8] = 0;
    cardMesh.texcoords[i*8 + 1] = 0;

    cardMesh.texcoords[i*8 + 2] = 1;
    cardMesh.texcoords[i*8 + 3] = 0;

    cardMesh.texcoords[i*8 + 4] = 1;
    cardMesh.texcoords[i*8 + 5] = 0.5;

    cardMesh.texcoords[i*8 + 6] = 0;
    cardMesh.texcoords[i*8 + 7] = 0.5;

    cardMesh.texcoords[j*8] = 0;
    cardMesh.texcoords[j*8 + 1] = 0.5;

    cardMesh.texcoords[j*8 + 2] = 0;
    cardMesh.texcoords[j*8 + 3] = 1;

    cardMesh.texcoords[j*8 + 4] = 1;
    cardMesh.texcoords[j*8 + 5] = 1;

    cardMesh.texcoords[j*8 + 6] = 1;
    cardMesh.texcoords[j*8 + 7] = 0.5;
    
    Model cardModel = LoadModelFromMesh(CopyMesh(cardMesh));
    UnloadMesh(cardMesh);
    return cardModel;
}

map<CardCategory, Texture2D> CardView::cardTextureMap;

CardView::CardView(CardPileView* pile, const Vector2& cardSize, const Vector3& pos) : View3D(pos, 
    createCardModel(cardSize), WHITE), 
    cardTexture(LoadRenderTexture(cardTextureMap[pile->getPileCategory()].width, 
                cardTextureMap[pile->getPileCategory()].height)) {
    string decalPath;
    BeginTextureMode(cardTexture);
    DrawTextureEx(cardTextureMap[pile->getPileCategory()], {0,0}, 0, 1, WHITE);
    EndTextureMode();
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = cardTexture.texture;
    transformation = MatrixRotateY(5*M_PI/4);
    transformation = MatrixRotateX(M_PI)*transformation;
}

void CardView::renderContent(Card& card) {
    string cardHeader;
    if (card.getCategory() == CardCategory::CHANCE) {
        cardHeader = "CHANCE";
    } else if (card.getCategory() == CardCategory::COMMUNITY) {
        cardHeader = "COMMUNITY CHEST";
    }
    Vector2 textureDim = {(float)cardTexture.texture.width, (float)cardTexture.texture.height};
    Vector2 textDim = MeasureTextEx(View2D::getFont("Kabel"), cardHeader.c_str(), 36, 1);
    RenderTexture2D textTexture = LoadRenderTexture(textDim.x, textDim.y);
    BeginTextureMode(textTexture);
    drawTextLinesWrapped(View2D::getFont("Kabel"), cardHeader, {textureDim.x/2, textureDim.y*3/8}, 36, 1, BLACK, textureDim);
    EndTextureMode();
    Image textImg = LoadImageFromTexture(textTexture.texture);
    ImageFlipVertical(&textImg);
    BeginTextureMode(cardTexture);
    
    EndTextureMode();
}

void CardView::loadCardTextures() {
    cardTextureMap[CardCategory::CHANCE] = LoadTexture("data/GUIAssets/chance_template.png");
    cardTextureMap[CardCategory::COMMUNITY] = LoadTexture("data/GUIAssets/comchest_template.png");
    cardTextureMap[CardCategory::SKILL] = LoadTexture("data/GUIAssets/skill_template.png");
}