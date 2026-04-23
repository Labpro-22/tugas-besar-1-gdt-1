#pragma once

#include "Interactable.hpp"

class Player;

class PlayerProfileView : public Interactable {
private:
    Player* player;
    std::string profileImgPath;
    Texture2D profileTexture;
    bool textureLoaded;

public:
    PlayerProfileView(Player* player = nullptr);
    ~PlayerProfileView();

    void render() override;
    void onHover() override;
    void onClicked() override;
    void setProfileImg(std::string imgPath);
};