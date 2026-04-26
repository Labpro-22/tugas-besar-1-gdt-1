#include "views/viewElement/player/PlayerInventory.hpp"

#include "models/Property/Property.hpp"
#include "models/Property/StreetProperty.hpp"
#include "models/Property/RailroadProperty.hpp"
#include "models/Property/UtilityProperty.hpp"
#include "models/CardAndDeck/SkillCard.hpp"
#include "views/viewElement/property/PropertyPopup.hpp"

#include <string>
#include <vector>
#include <sstream>
#include <map>

static std::string propertyTypeToString(PropertyType type)
{
    switch (type)
    {
    case PropertyType::STREET:
        return "STREET";
    case PropertyType::RAILROAD:
        return "RAILROAD";
    case PropertyType::UTILITY:
        return "UTILITY";
    default:
        return "UNKNOWN";
    }
}

static std::vector<int> mapValuesAscending(const std::map<int, int> &source)
{
    std::vector<int> values;
    values.reserve(source.size());
    for (const auto &entry : source)
    {
        values.push_back(entry.second);
    }
    return values;
}

static std::string mapPropertyPopupCommand(
    const std::string &popupCommand,
    const std::string &propertyCode)
{
    if (propertyCode.empty())
    {
        return "NULL";
    }

    if (popupCommand == "BUILD_PROPERTY")
    {
        return "BANGUN " + propertyCode;
    }
    if (popupCommand == "MORTGAGE_PROPERTY")
    {
        return "GADAI " + propertyCode;
    }
    if (popupCommand == "REDEEM_PROPERTY")
    {
        return "TEBUS " + propertyCode;
    }

    return "NULL";
}

static std::vector<std::string> wrapTextToWidth(
    const std::string& text,
    Font font,
    float fontSize,
    float spacing,
    float maxWidth
)
{
    std::vector<std::string> lines;
    std::stringstream ss(text);
    std::string word;
    std::string currentLine;

    while (ss >> word)
    {
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        float testWidth = MeasureTextEx(font, testLine.c_str(), fontSize, spacing).x;

        if (testWidth > maxWidth && !currentLine.empty())
        {
            lines.push_back(currentLine);
            currentLine = word;
        }
        else
        {
            currentLine = testLine;
        }
    }

    if (!currentLine.empty())
        lines.push_back(currentLine);

    return lines;
}

static float drawWrappedText(
    const std::string& text,
    Font font,
    Vector2 pos,
    float fontSize,
    float spacing,
    float maxWidth,
    Color color,
    float lineGap = 4.0f
)
{
    std::vector<std::string> lines = wrapTextToWidth(text, font, fontSize, spacing, maxWidth);

    float currentY = pos.y;
    for (const std::string& line : lines)
    {
        DrawTextEx(font, line.c_str(), {pos.x, currentY}, fontSize, spacing, color);
        currentY += fontSize + lineGap;
    }

    return currentY;
}

PlayerInventoryPopup::PlayerInventoryPopup(Player* player)
    : IndefinitePopup(View2D(getScreenCenter(), {760, 520}, [](){})),
      player(player)
{
    exitButton = Interactable(
        {30, 30},
        true,
        false,
        "CLOSE_INVENTORY",
        [](){},
        [this](){ close(); }
    );

    exitButton.setRender([this]()
    {
        Rectangle rect = {
            exitButton.getRenderPos().x,
            exitButton.getRenderPos().y,
            exitButton.getRenderWidth(),
            exitButton.getRenderHeight()
        };

        bool hovered = CheckCollisionPointRec(GetMousePosition(), rect);

        DrawRectangleRounded(
            rect,
            0.25f,
            8,
            hovered ? Color{220, 220, 220, 255} : Color{245, 245, 245, 255}
        );

        DrawRectangleRoundedLinesEx(
            rect,
            0.25f,
            8,
            1.5f,
            Color{40, 40, 40, 255}
        );

        DrawLineEx(
            {rect.x + 8, rect.y + 8},
            {rect.x + rect.width - 8, rect.y + rect.height - 8},
            2.5f,
            BLACK
        );

        DrawLineEx(
            {rect.x + rect.width - 8, rect.y + 8},
            {rect.x + 8, rect.y + rect.height - 8},
            2.5f,
            BLACK
        );
    });
}

void PlayerInventoryPopup::enable()
{
    exitButton.enable();
    for (auto &button : cardUseButtons)
        button.enable();
    for (auto &button : propertyInfoButtons)
        button.enable();
    if (propertyPopup)
        propertyPopup->enable();
}

void PlayerInventoryPopup::disable()
{
    exitButton.disable();
    for (auto &button : cardUseButtons)
        button.disable();
    for (auto &button : propertyInfoButtons)
        button.disable();
    if (propertyPopup)
        propertyPopup->disable();
}

void PlayerInventoryPopup::interactionCheck()
{
    if (propertyPopup)
    {
        propertyPopup->interactionCheck();
        if (propertyPopup->closed() && propertyPopup->catchCommand() == "NULL")
        {
            propertyPopup.reset();
            selectedPropertyCode.clear();
        }
        return;
    }

    std::vector<Property *> displayedProperties;
    if (player != nullptr)
    {
        const std::vector<Property *> &properties = player->getOwnedProperties();
        for (Property *property : properties)
        {
            if (property == nullptr)
                continue;
            if (displayedProperties.size() >= 10)
                break;
            displayedProperties.push_back(property);
        }
    }

    for (auto &button : propertyInfoButtons)
    {
        button.interactionCheck();

        std::string cmd = button.catchCommand();
        const std::string prefix = "PROPERTY_INFO_";
        if (cmd.rfind(prefix, 0) == 0)
        {
            int idx = -1;
            try
            {
                idx = std::stoi(cmd.substr(prefix.size()));
            }
            catch (...)
            {
                idx = -1;
            }

            if (idx >= 0 && idx < static_cast<int>(displayedProperties.size()))
            {
                Property *property = displayedProperties[idx];
                if (player != nullptr && property != nullptr)
                {
                    std::vector<int> rentTable;
                    std::vector<int> railroadRent;
                    std::vector<int> utilityMultiplier;
                    std::string colorGroup;
                    int buildCost = 0;
                    int levelOrCount = 0;

                    if (property->getType() == PropertyType::STREET)
                    {
                        StreetProperty *street = static_cast<StreetProperty *>(property);
                        colorGroup = street->getColorGroup();
                        rentTable = street->getRentLevels();
                        buildCost = street->getHouseBuildCost();
                        levelOrCount = static_cast<int>(street->getBuildingState());
                    }
                    else if (property->getType() == PropertyType::RAILROAD)
                    {
                        RailroadProperty *railroad = static_cast<RailroadProperty *>(property);
                        railroadRent = mapValuesAscending(railroad->getRentTable());

                        int count = 0;
                        for (Property *owned : player->getOwnedProperties())
                        {
                            if (owned != nullptr && owned->getType() == PropertyType::RAILROAD)
                                count++;
                        }
                        levelOrCount = count;
                    }
                    else if (property->getType() == PropertyType::UTILITY)
                    {
                        UtilityProperty *utility = static_cast<UtilityProperty *>(property);
                        utilityMultiplier = mapValuesAscending(utility->getMultiplierTable());

                        int count = 0;
                        for (Property *owned : player->getOwnedProperties())
                        {
                            if (owned != nullptr && owned->getType() == PropertyType::UTILITY)
                                count++;
                        }
                        levelOrCount = count;
                    }

                    propertyPopup = std::make_unique<PropertyPopup>(
                        property->getName(),
                        propertyTypeToString(property->getType()),
                        Property::propertyStatusToString(property->getStatus()),
                        property->getPurchasePrice(),
                        property->getMortgageValue(),
                        levelOrCount,
                        property->getOwner() != player,
                        property->getOwner() ? property->getOwner()->getUsername() : "",
                        colorGroup,
                        rentTable,
                        buildCost,
                        railroadRent,
                        utilityMultiplier);

                    selectedPropertyCode = property->getCode();
                    propertyPopup->enable();
                    break;
                }
            }
        }
    }

    for (auto &button : cardUseButtons)
    {
        button.interactionCheck();
    }

    exitButton.interactionCheck();
}

std::string PlayerInventoryPopup::catchCommand()
{
    if (propertyPopup)
    {
        std::string popupCommand = propertyPopup->catchCommand();
        std::string mappedCommand = mapPropertyPopupCommand(popupCommand, selectedPropertyCode);
        if (mappedCommand != "NULL")
        {
            propertyPopup.reset();
            selectedPropertyCode.clear();
            return mappedCommand;
        }

        if (propertyPopup->closed())
        {
            propertyPopup.reset();
            selectedPropertyCode.clear();
        }
    }

    for (auto &button : cardUseButtons)
    {
        std::string cmd = button.catchCommand();
        const std::string prefix = "USE_CARD_IDX_";
        if (cmd.rfind(prefix, 0) == 0)
        {
            std::string cardNumber = cmd.substr(prefix.size());
            close();
            return "GUNAKAN_KEMAMPUAN " + cardNumber;
        }
    }

    return "NULL";
}

void PlayerInventoryPopup::render()
{
    animationCheck();

    float x = getRenderPos().x;
    float y = getRenderPos().y;
    float w = getRenderWidth();
    float h = getRenderHeight();

    exitButton.movePosition({x + w - 30, y + 30});

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
    DrawTextEx(fontMap["Orbitron"], "Skill Cards", {leftX, topY + 100}, 22, 1, DARKGREEN);
    DrawTextEx(fontMap["Orbitron"], "Properties", {rightX, topY}, 22, 1, MAROON);

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

    const std::vector<SkillCard*>& cards = player->getHandCards();
    float cardY = topY + 135;
    float cardMaxWidth = x + (w / 2 - 24) - leftX;
    int renderedCardButtons = 0;

    if (cards.empty())
    {
        DrawTextEx(fontMap["Orbitron"], "- No skill cards", {leftX, cardY}, 16, 1, GRAY);
    }
    else
    {
        int shown = 0;
        for (size_t handIndex = 0; handIndex < cards.size(); ++handIndex)
        {
            SkillCard *card = cards[handIndex];
            if (card == nullptr) continue;
            if (shown >= 10) break;

            std::string cardName = "- " + card->getCardName();

            DrawTextEx(
                fontMap["Orbitron"],
                cardName.c_str(),
                {leftX, cardY},
                15,
                1,
                BLACK
            );

            if (cardUseButtons.size() <= static_cast<size_t>(renderedCardButtons))
            {
                cardUseButtons.emplace_back(
                    Vector2{52, 20},
                    true,
                    false,
                    "USE_CARD_IDX_" + std::to_string(handIndex + 1),
                    []() {},
                    []() {});
            }

            auto &useBtn = cardUseButtons[renderedCardButtons];
            useBtn.setGameCommand("USE_CARD_IDX_" + std::to_string(handIndex + 1));
            useBtn.setHitboxDim({52, 20});
            useBtn.enable();
            useBtn.movePosition({leftX + cardMaxWidth - 26, cardY + 8});
            useBtn.setRender([this, renderedCardButtons]()
                             {
                auto &btn = cardUseButtons[renderedCardButtons];
                Rectangle rect = btn.getHitbox();
                bool hovered = CheckCollisionPointRec(GetMousePosition(), rect);

                DrawRectangleRounded(
                    rect,
                    0.25f,
                    6,
                    hovered ? Color{225, 240, 225, 255} : Color{235, 248, 235, 255}
                );
                DrawRectangleRoundedLinesEx(
                    rect,
                    0.25f,
                    6,
                    1.2f,
                    Color{55, 95, 55, 255}
                );

                const char *txt = "USE";
                int fs = 12;
                int tw = MeasureText(txt, fs);
                DrawText(
                    txt,
                    rect.x + (rect.width - tw) / 2,
                    rect.y + (rect.height - fs) / 2,
                    fs,
                    Color{25, 75, 25, 255}
                ); });
            useBtn.render();
            renderedCardButtons++;

            cardY += 20;

            if (!card->getDescription().empty())
            {
                cardY = drawWrappedText(
                    card->getDescription(),
                    fontMap["Orbitron"],
                    {leftX + 16, cardY},
                    13,
                    1,
                    cardMaxWidth - 16,
                    DARKGRAY,
                    3
                );
            }

            cardY += 10;
            shown++;
        }

        if ((int)cards.size() > 10)
        {
            DrawTextEx(fontMap["Orbitron"], "... more cards omitted", {leftX, cardY}, 14, 1, GRAY);
        }
    }

    if (cardUseButtons.size() > static_cast<size_t>(renderedCardButtons))
    {
        cardUseButtons.resize(renderedCardButtons);
    }

    const std::vector<Property *> &properties = player->getOwnedProperties();
    float propY = topY + 35;
    float rightPanelRight = x + w - 24;
    int renderedPropertyCount = 0;

    if (properties.empty())
    {
        DrawTextEx(fontMap["Orbitron"], "- No properties", {rightX, propY}, 16, 1, GRAY);
    }
    else
    {
        int shown = 0;
        for (Property *property : properties)
        {
            if (property == nullptr)
                continue;
            if (shown >= 10)
                break;

            std::string typeText = propertyTypeToString(property->getType());
            std::string statusText = Property::propertyStatusToString(property->getStatus());

            std::string line =
                "- " + property->getCode() +
                " | " + property->getName() +
                " | " + typeText +
                " | " + statusText;

            DrawTextEx(fontMap["Orbitron"], line.c_str(), {rightX, propY}, 15, 1, BLACK);

            int propertyIndex = shown;

            if (propertyInfoButtons.size() <= static_cast<size_t>(propertyIndex))
            {
                propertyInfoButtons.emplace_back(
                    Vector2{56, 22},
                    true,
                    false,
                    "PROPERTY_INFO_" + std::to_string(propertyIndex),
                    []() {},
                    []() {});
            }

            auto &infoBtn = propertyInfoButtons[propertyIndex];
            infoBtn.setGameCommand("PROPERTY_INFO_" + std::to_string(propertyIndex));
            infoBtn.setHitboxDim({56, 22});
            infoBtn.enable();
            infoBtn.movePosition({rightPanelRight - 28, propY + 8});
            infoBtn.setRender([this, propertyIndex]()
                              {
                auto &btn = propertyInfoButtons[propertyIndex];
                Rectangle rect = btn.getHitbox();
                bool hovered = CheckCollisionPointRec(GetMousePosition(), rect);

                DrawRectangleRounded(
                    rect,
                    0.25f,
                    6,
                    hovered ? Color{220, 230, 250, 255} : Color{235, 240, 250, 255}
                );
                DrawRectangleRoundedLinesEx(
                    rect,
                    0.25f,
                    6,
                    1.2f,
                    Color{70, 80, 100, 255}
                );

                const char *txt = "+info";
                int fs = 12;
                int tw = MeasureText(txt, fs);
                DrawText(
                    txt,
                    rect.x + (rect.width - tw) / 2,
                    rect.y + (rect.height - fs) / 2,
                    fs,
                    Color{25, 35, 55, 255}
                ); });

            infoBtn.render();

            propY += 20;

            if (property->getType() == PropertyType::STREET)
            {
                StreetProperty *street = static_cast<StreetProperty *>(property);

                std::string buildText = "No Building";
                if (street->getBuildingState() == BuildingState::HOUSE_1)
                    buildText = "1 House";
                else if (street->getBuildingState() == BuildingState::HOUSE_2)
                    buildText = "2 Houses";
                else if (street->getBuildingState() == BuildingState::HOUSE_3)
                    buildText = "3 Houses";
                else if (street->getBuildingState() == BuildingState::HOUSE_4)
                    buildText = "4 Houses";
                else if (street->getBuildingState() == BuildingState::HOTEL)
                    buildText = "Hotel";

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

                DrawTextEx(fontMap["Orbitron"], detail.c_str(), {rightX, propY}, 14, 1, DARKGRAY);
                propY += 20;
            }

            shown++;
        }
        renderedPropertyCount = shown;

        if ((int)properties.size() > 10)
        {
            DrawTextEx(fontMap["Orbitron"], "... more properties omitted", {rightX, propY}, 14, 1, GRAY);
        }
    }

    if (propertyInfoButtons.size() > static_cast<size_t>(renderedPropertyCount))
    {
        propertyInfoButtons.resize(renderedPropertyCount);
    }

    exitButton.render();

    if (propertyPopup)
    {
        propertyPopup->render();
    }

}