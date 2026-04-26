#include "views/viewElement/player/PlayerInventory.hpp"

#include "models/Property/Property.hpp"
#include "models/Property/StreetProperty.hpp"
#include "models/CardAndDeck/SkillCard.hpp"

#include <string>
#include <vector>

PlayerInventoryPopup::PlayerInventoryPopup(Player* player)
    : IndefinitePopup(View2D(getScreenCenter(), {760, 520}, [](){})),
      player(player)
{
}

void PlayerInventoryPopup::enable()
{
    exitButton.enable();
}

void PlayerInventoryPopup::disable()
{
    exitButton.disable();
}

void PlayerInventoryPopup::interactionCheck()
{
    exitButton.interactionCheck();
}

std::string PlayerInventoryPopup::catchCommand()
{
    return "NULL";
}

void PlayerInventoryPopup::render()
{
    animationCheck();

    float x = getRenderPos().x;
    float y = getRenderPos().y;
    float w = getRenderWidth();
    float h = getRenderHeight();

    DrawRectangleRounded({x, y, w, h}, 0.06f, 12, RAYWHITE);
    DrawRectangleRoundedLinesEx({x, y, w, h}, 0.06f, 12, 2, DARKGRAY);
    DrawRectangleRounded({x, y, w, 70}, 0.06f, 12, DARKBLUE);

    std::string title = "PLAYER INVENTORY";
    if (player != nullptr)
    {
        title = player->getUsername() + " - Inventory";
    }

    DrawTextEx(
        fontMap["Orbitron"],
        title.c_str(),
        {x + 24, y + 20},
        28,
        1,
        WHITE
    );

    float leftX = x + 24;
    float rightX = x + w / 2 + 10;
    float topY = y + 90;

    DrawLineEx(
        {x + w / 2 - 10, y + 84},
        {x + w / 2 - 10, y + h - 20},
        2.0f,
        LIGHTGRAY
    );

    DrawTextEx(fontMap["Orbitron"], "Summary", {leftX, topY}, 22, 1, MAROON);
    DrawTextEx(fontMap["Orbitron"], "Properties", {leftX, topY + 100}, 22, 1, MAROON);
    DrawTextEx(fontMap["Orbitron"], "Skill Cards", {rightX, topY}, 22, 1, DARKGREEN);

    if (player == nullptr)
    {
        DrawTextEx(
            fontMap["Orbitron"],
            "Player data is null.",
            {leftX, topY + 40},
            18,
            1,
            GRAY
        );

        exitButton.render();
        return;
    }

    std::string usernameLine = "Username : " + player->getUsername();
    std::string balanceLine = "Balance  : $" + std::to_string(player->getBalance());
    std::string cardCountLine = "Cards    : " + std::to_string(player->getCardCount());

    DrawTextEx(fontMap["Orbitron"], usernameLine.c_str(), {leftX, topY + 35}, 18, 1, BLACK);
    DrawTextEx(fontMap["Orbitron"], balanceLine.c_str(), {leftX, topY + 60}, 18, 1, BLACK);
    DrawTextEx(fontMap["Orbitron"], cardCountLine.c_str(), {leftX, topY + 85}, 18, 1, BLACK);

    const std::vector<Property*>& properties = player->getOwnedProperties();
    float propY = topY + 135;

    if (properties.empty())
    {
        DrawTextEx(fontMap["Orbitron"], "- No properties", {leftX, propY}, 16, 1, GRAY);
    }
    else
    {
        int shown = 0;
        for (Property* property : properties)
        {
            if (property == nullptr) continue;
            if (shown >= 10) break;

            std::string typeText = "UNKNOWN";
            if (property->getType() == PropertyType::STREET) typeText = "STREET";
            else if (property->getType() == PropertyType::RAILROAD) typeText = "RAILROAD";
            else if (property->getType() == PropertyType::UTILITY) typeText = "UTILITY";

            std::string statusText = "UNKNOWN";
            if (property->getStatus() == PropertyStatus::BANK) statusText = "BANK";
            else if (property->getStatus() == PropertyStatus::OWNED) statusText = "OWNED";
            else if (property->getStatus() == PropertyStatus::MORTGAGED) statusText = "MORTGAGED";

            std::string line =
                "- " + property->getCode() +
                " | " + property->getName() +
                " | " + typeText +
                " | " + statusText;

            DrawTextEx(fontMap["Orbitron"], line.c_str(), {leftX, propY}, 15, 1, BLACK);
            propY += 20;

            if (property->getType() == PropertyType::STREET)
            {
                StreetProperty* street = static_cast<StreetProperty*>(property);

                std::string buildText = "No Building";
                if (street->getBuildingState() == BuildingState::HOUSE_1) buildText = "1 House";
                else if (street->getBuildingState() == BuildingState::HOUSE_2) buildText = "2 Houses";
                else if (street->getBuildingState() == BuildingState::HOUSE_3) buildText = "3 Houses";
                else if (street->getBuildingState() == BuildingState::HOUSE_4) buildText = "4 Houses";
                else if (street->getBuildingState() == BuildingState::HOTEL) buildText = "Hotel";

                std::string detail =
                    "   Color: " + street->getColorGroup() +
                    " | Build: " + buildText;

                if (property->getFestivalMultiplier() > 1 &&
                    property->getFestivalDuration() > 0)
                {
                    detail +=
                        " | Festival x" + std::to_string(property->getFestivalMultiplier()) +
                        " (" + std::to_string(property->getFestivalDuration()) + " turn)";
                }

                DrawTextEx(fontMap["Orbitron"], detail.c_str(), {leftX, propY}, 14, 1, DARKGRAY);
                propY += 20;
            }

            shown++;
        }

        if ((int)properties.size() > 10)
        {
            DrawTextEx(fontMap["Orbitron"], "... more properties omitted", {leftX, propY}, 14, 1, GRAY);
        }
    }

    const std::vector<SkillCard*>& cards = player->getHandCards();
    float cardY = topY + 35;

    if (cards.empty())
    {
        DrawTextEx(fontMap["Orbitron"], "- No skill cards", {rightX, cardY}, 16, 1, GRAY);
    }
    else
    {
        int shown = 0;
        for (SkillCard* card : cards)
        {
            if (card == nullptr) continue;
            if (shown >= 10) break;

            std::string line = "- " + card->getCardName();
            if (!card->getDescription().empty())
            {
                line += " | " + card->getDescription();
            }

            DrawTextEx(fontMap["Orbitron"], line.c_str(), {rightX, cardY}, 15, 1, BLACK);
            cardY += 22;
            shown++;
        }

        if ((int)cards.size() > 10)
        {
            DrawTextEx(fontMap["Orbitron"], "... more cards omitted", {rightX, cardY}, 14, 1, GRAY);
        }
    }

    exitButton.render();
}