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

CardView::CardView(Card* card, const Vector2& cardSize, const Vector3& pos) : View3D(pos, 
    createCardModel(cardSize), WHITE), card(card) {
}

ActionCardView::ActionCardView(Card* card, CardPileView* pile, const Vector3& pos) : 
    CardView(card, pile->getCardSize(), pos), pile(pile){
    string decalPath;
    string cardHeader;
    switch(card->getCategory()) {
        case CardCategory::CHANCE :
            decalPath = "data/GUIAssets/chance_template.png";
            cardHeader = "CHANCE";
            break;
        case CardCategory::COMMUNITY :
            decalPath = "data/GUIAssets/comchest_template.png";
            cardHeader = "COMMUNITY CHEST";
            break;
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

SkillCardView::SkillCardView(Card* card, const Vector3& pos) : CardView(card, {6.4f, 3.6f}, pos) {
    Texture2D cText = LoadTexture("data/GUIAssets/skill_template.png");
    cardTexture = LoadRenderTexture(cText.width, cText.height);
    Vector2 textureDim = {(float)cardTexture.texture.height, (float)cardTexture.texture.width};
    RenderTexture2D textTexture = LoadRenderTexture(textureDim.x, textureDim.y);
    BeginTextureMode(textTexture);
    drawTextLinesWrapped(View2D::getFont("Kabel"), card->getCardName(), {textureDim.x*0.75f,textureDim.y*0.55f}, 64, 1, BLACK, textureDim*0.4f);
    drawTextLinesWrapped(View2D::getFont("Kabel"), card->getDescription(), {textureDim.x*0.75f,textureDim.y*0.9f}, 56, 1, BLACK, textureDim*0.4f);
    EndTextureMode();

    BeginTextureMode(cardTexture);
    DrawTextureEx(cText, {0,0}, 0, 1, WHITE);
    DrawTextureEx(textTexture.texture, {textureDim.x/2,-textureDim.y*0.575f}, 90, 1, WHITE);
    EndTextureMode();
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = cardTexture.texture;
    transformation = MatrixRotateY(M_PI);
}

Card* CardView::getCard() const {
    return card;
}


ActionCardView::~ActionCardView() {
    UnloadTexture(cardTexture.texture);
    UnloadModel(model);
}

SkillCardView::~SkillCardView() {
    UnloadTexture(cardTexture.texture);
    UnloadModel(model);
}

