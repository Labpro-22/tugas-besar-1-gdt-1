#pragma once

#include "views/viewElement/Interactable.hpp"

class Player;

class PlayerProfileView : public Interactable {
private:
    Player* player;
    Color playerColor;

public:
    PlayerProfileView(Player* player = nullptr);

    void render() override;
    void onHover() override;
    void onClicked() override;
    
    void setColor(Color c);
    void setPlayer(Player* p);

    void setPlayer(Player* p);
};