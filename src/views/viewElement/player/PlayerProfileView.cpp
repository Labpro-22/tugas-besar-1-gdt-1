#include "views/viewElement/player/PlayerProfileView.hpp"
#include "models/Player/Player.hpp"

PlayerProfileView::PlayerProfileView(Player *player)
    : player(player), playerColor(LIGHTGRAY)
{
    setActive(true);
    setGameCommand("SHOW_PLAYER_INVENTORY");
}

Player* PlayerProfileView::getPlayer() const {
    return player;
}

void PlayerProfileView::render()
{
    animationCheck();
    if (!player)
        return;

    Rectangle rect = getHitbox();

    DrawRectangleRounded(
        {rect.x + 4, rect.y + 4, rect.width, rect.height},
        0.2f, 8, Fade(BLACK, 0.3f));

    DrawRectangleRounded(rect, 0.2f, 8, DARKGRAY);

    DrawRectangleRounded(
        {rect.x, rect.y, 10, rect.height},
        0.2f, 8, playerColor);

    DrawText(player->getUsername().c_str(),
             rect.x + 16,
             rect.y + 10,
             20,
             WHITE);

    std::string money = "$" + std::to_string(player->getBalance());

    DrawText(money.c_str(),
             rect.x + 16,
             rect.y + 40,
             22,
             GOLD);
}

void PlayerProfileView::setColor(Color c)
{
    playerColor = c;
}

void PlayerProfileView::setPlayer(Player *p)
{
    this->player = p;
}

void PlayerProfileView::onHover()
{
    DrawRectangleLinesEx(getHitbox(), 6, Fade(playerColor, 0.5f));
}

void PlayerProfileView::onClicked()
{
    Interactable::onClicked();
}