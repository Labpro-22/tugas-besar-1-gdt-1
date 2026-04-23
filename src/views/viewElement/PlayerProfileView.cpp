#include "views/viewElement/PlayerProfileView.hpp"
#include "models/Player/Player.hpp"


#include <filesystem>
#include <sstream>

PlayerProfileView::PlayerProfileView(Player* player)
    : Interactable(),
      player(player),
      profileImgPath("data/GUIAssets/Profile/default.png"),
      profileTexture({0}),
      textureLoaded(false) {
    boundingDim = {130.0f, 135.0f};
    hitboxDim = boundingDim;

    active = true;
    draggable = false;
    releaseCommand = false;
    gameCommand = "";

    if (std::filesystem::exists(profileImgPath)) {
        profileTexture = LoadTexture(profileImgPath.c_str());
        textureLoaded = (profileTexture.id != 0);
    }
}

PlayerProfileView::~PlayerProfileView() {
    if (textureLoaded) {
        UnloadTexture(profileTexture);
        textureLoaded = false;
    }
}

void PlayerProfileView::setProfileImg(std::string imgPath) {
    if (textureLoaded) {
        UnloadTexture(profileTexture);
        textureLoaded = false;
    }

    profileImgPath = imgPath;

    if (profileImgPath.empty() || !std::filesystem::exists(profileImgPath)) {
        profileImgPath = "data/GUIAssets/Profile/default.png";
    }

    if (std::filesystem::exists(profileImgPath)) {
        profileTexture = LoadTexture(profileImgPath.c_str());
        textureLoaded = (profileTexture.id != 0);
    }
}

void PlayerProfileView::render() {
    if (!visible) return;

    std::string username = "Unknown";
    int balance = 0;
    int cardCount = 0;

    if (player != nullptr) {
        username = player->getUsername();
        balance = player->getBalance();
        cardCount = player->getCardCount();
    }
    std::string displayName = username;
    if (displayName.length() > 10) {
        displayName = displayName.substr(0, 10);
    }

    Font font = GetFontDefault();
    if (fontMap.count("Orbitron")) {
        font = fontMap.at("Orbitron");
    } else if (fontMap.count("default")) {
        font = fontMap.at("default");
    }

    Vector2 topLeft = getRenderPos();
    float x = topLeft.x;
    float y = topLeft.y;

    // =========================================
    // Layout compact
    // =========================================
    float nameBarW = 108.0f * scale;
    float nameBarH = 22.0f * scale;

    float avatarSize = 82.0f * scale;
    float avatarX = x + 12.0f * scale;
    float avatarY = y + 30.0f * scale;

    float badgeW = 28.0f * scale;
    float badgeH = 36.0f * scale;
    float badgeX = avatarX + avatarSize + 10.0f * scale;
    float badgeY = avatarY + 44.0f * scale;

    // =========================================
    // Name bar
    // =========================================

    Color nameBarColor = Color{226, 163, 90, 255};
    Color nameTextColor = BLACK;

    DrawRectangleRounded(
        Rectangle{x, y, nameBarW, nameBarH},
        0.20f,
        8,
        nameBarColor
    );

    float nameFont = getRenderFontSize(15.0f);
    Vector2 nameDim = MeasureTextEx(font, displayName.c_str(), nameFont, 1.0f);

    // kecilkan font kalau terlalu panjang
    while (nameDim.x > nameBarW - 16.0f * scale && nameFont > 9.0f * scale) {
        nameFont -= 1.0f;
        nameDim = MeasureTextEx(font, displayName.c_str(), nameFont, 1.0f);
    }

    // left align + vertical center
    float textPaddingLeft = 8.0f * scale;
    float visualYOffset = -2.0f * scale;
    float textY = y + (nameBarH - nameDim.y) / 2.0f + visualYOffset;

    DrawTextEx(
        font,
        displayName.c_str(),
        Vector2{
            x + textPaddingLeft,
            textY
        },
        nameFont,
        1.0f,
        nameTextColor
    );

    // =========================================
    // Avatar frame
    // =========================================
    DrawRectangleRounded(
        Rectangle{avatarX, avatarY, avatarSize, avatarSize},
        0.08f,
        8,
        WHITE
    );

    Rectangle innerAvatarRect = {
        avatarX + 4.0f * scale,
        avatarY + 4.0f * scale,
        avatarSize - 8.0f * scale,
        avatarSize - 8.0f * scale
    };

    if (textureLoaded) {
        DrawTexturePro(
            profileTexture,
            Rectangle{
                0,
                0,
                static_cast<float>(profileTexture.width),
                static_cast<float>(profileTexture.height)
            },
            innerAvatarRect,
            Vector2{0, 0},
            0.0f,
            WHITE
        );
    } else {
        DrawRectangleRounded(
            innerAvatarRect,
            0.08f,
            8,
            LIGHTGRAY
        );

        float noImgFont = getRenderFontSize(11.0f);
        const char* fallbackText = "NO IMG";
        Vector2 noImgDim = MeasureTextEx(font, fallbackText, noImgFont, 1.0f);

        DrawTextEx(
            font,
            fallbackText,
            Vector2{
                innerAvatarRect.x + (innerAvatarRect.width - noImgDim.x) / 2.0f,
                innerAvatarRect.y + (innerAvatarRect.height - noImgDim.y) / 2.0f
            },
            noImgFont,
            1.0f,
            DARKGRAY
        );
    }

    // =========================================
    // Badge kartu model tumpukan
    // =========================================
    DrawRectangleRounded(
        Rectangle{
            badgeX - 6.0f * scale,
            badgeY - 4.0f * scale,
            badgeW,
            badgeH
        },
        0.12f,
        6,
        Color{244, 229, 163, 255}
    );

    DrawRectangleRoundedLinesEx(
        Rectangle{
            badgeX - 6.0f * scale,
            badgeY - 4.0f * scale,
            badgeW,
            badgeH
        },
        0.12f,
        6,
        1.0f * scale,
        ColorAlpha(BLACK, 0.15f)
    );

    DrawRectangleRounded(
        Rectangle{badgeX, badgeY, badgeW, badgeH},
        0.12f,
        6,
        Color{253, 240, 177, 255}
    );

    DrawRectangleRoundedLinesEx(
        Rectangle{badgeX, badgeY, badgeW, badgeH},
        0.12f,
        6,
        1.0f * scale,
        ColorAlpha(BLACK, 0.25f)
    );
    //cardCount
    std::string badgeText = std::to_string(36);

    float badgeFont = getRenderFontSize(17.0f);
    float badgeTextPaddingRight = scale;
    if (badgeText.length() >= 2) {
        badgeFont = getRenderFontSize(14.0f);
    }
    if (badgeText.length() >= 3) {
        badgeFont = getRenderFontSize(12.0f);

    }

    Vector2 badgeDim = MeasureTextEx(font, badgeText.c_str(), badgeFont, 1.0f);

    float badgeTextY = badgeY + (badgeH - badgeDim.y) / 2.0f + 8.0f * scale;

    DrawTextEx(
        font,
        badgeText.c_str(),
        Vector2{
            badgeX + badgeW - badgeDim.x - badgeTextPaddingRight,
            badgeTextY
        },
        badgeFont,
        1.0f,
        BLACK
    );

    // =========================================
    // Money di bawah avatar
    // =========================================
    std::stringstream balanceSs;
    balanceSs << "$" << balance;
    std::string balanceText = balanceSs.str();

    float balanceFont = getRenderFontSize(15.0f);
    Vector2 balanceDim = MeasureTextEx(font, balanceText.c_str(), balanceFont, 1.0f);

    DrawTextEx(
        font,
        balanceText.c_str(),
        Vector2{
            avatarX + (avatarSize - balanceDim.x) / 2.0f,
            avatarY + avatarSize + 10.0f * scale
        },
        balanceFont,
        1.0f,
        BLACK
    );

    // =========================================
    // Sync hitbox / bounding
    // =========================================
    boundingDim = {
        (badgeX + badgeW) - x,
        (avatarY + avatarSize + 28.0f * scale) - y
    };
    hitboxDim = boundingDim;
}

void PlayerProfileView::onHover() {
    // TODO: implement nanti
}

void PlayerProfileView::onClicked() {
    // TODO: implement nanti
}
