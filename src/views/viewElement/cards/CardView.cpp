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

CardView::CardView(Card* card, CardPileView* pile, const Vector2& cardSize, const Vector3& pos) : View3D(pos, 
    createCardModel(cardSize), WHITE), pile(pile), card(card) {
    string decalPath;
    string cardHeader;
    switch(pile->getPileCategory()) {
        case CardCategory::CHANCE :
            decalPath = "data/GUIAssets/chance_template.png";
            cardHeader = "CHANCE";
            break;
        case CardCategory::COMMUNITY :
            decalPath = "data/GUIAssets/comchest_template.png";
            cardHeader = "COMMUNITY CHEST";
            break;
        default :
            decalPath = "data/GUIAssets/skill_template.png";
            cardHeader = "SKILL";
    }
    Texture2D cText = LoadTexture(decalPath.c_str());
    cardTexture = LoadRenderTexture(cText.width, cText.height);
    Vector2 textureDim = {(float)cardTexture.texture.width, (float)cardTexture.texture.height};
    RenderTexture2D textTexture = LoadRenderTexture(textureDim.x, textureDim.y);
    
    BeginTextureMode(textTexture);
    drawTextLinesWrapped(View2D::getFont("Kabel"), cardHeader, {textureDim.x/2,textureDim.y*0.6f}, 72, 1, BLACK, textureDim*0.8f);
    drawTextLinesWrapped(View2D::getFont("Kabel"), card->getDescription(), {textureDim.x/2,textureDim.y*0.75f}, 48, 1, BLACK, textureDim*0.8f);
    EndTextureMode();

    BeginTextureMode(cardTexture);
    DrawTextureEx(cText, {0,0}, 0, 1, WHITE);
    DrawTextureEx(textTexture.texture, {0,0}, 0, 1, WHITE);
    EndTextureMode();
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = cardTexture.texture;
    transformation = MatrixRotateY(5*M_PI/4);
    transformation = MatrixRotateX(M_PI)*transformation;
}

Card* CardView::getCard() const {
    return card;
}

CardView::~CardView() {
    //UnloadTexture(cardTexture.texture);
}

