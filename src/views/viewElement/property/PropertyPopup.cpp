#include "views/viewElement/property/PropertyPopup.hpp"

#include <algorithm>
#include <sstream>

PropertyPopup::PropertyPopup(
    const std::string &name,
    const std::string &type,
    const std::string &status,
    int buyPrice,
    int mortgageValue,
    int levelOrCount,
    bool isOtherPlayer,
    const std::string &ownerName,
    const std::string &colorGroup,
    const std::vector<int> &rentTable,
    int buildCost,
    const std::vector<int> &railroadRent,
    const std::vector<int> &utilityMultiplier)
    : IndefinitePopup(View2D(getScreenCenter(), {520, 420}, []() {})),
      name(name),
      type(type),
      status(status),
      buyPrice(buyPrice),
      mortgageValue(mortgageValue),
      levelOrCount(levelOrCount),
      isOtherPlayer(isOtherPlayer),
      ownerName(ownerName),
      colorGroup(colorGroup),
      rentTable(rentTable),
      buildCost(buildCost),
      railroadRent(railroadRent),
      utilityMultiplier(utilityMultiplier),
      actionCommand("")
{
    if (status == "BANK")
    {
        if (type == "STREET")
        {
            addButton("BUY", "BUY_PROPERTY");
            addButton("SKIP", "SKIP_PROPERTY");
        }
        else
        {
            addButton("OK", "ACK");
        }
    }
    else if (status == "OWNED")
    {
        if (isOtherPlayer)
        {
            addButton("PAY", "PAY_RENT");
        }
        else
        {
            if (type == "STREET")
            {
                if (levelOrCount == 0)
                {
                    addButton("BUILD", "BUILD_PROPERTY");
                    addButton("MORTGAGE", "MORTGAGE_PROPERTY");
                }
                else if (levelOrCount < 5)
                {
                    addButton("BUILD", "BUILD_PROPERTY");
                }
            }
            else
            {
                addButton("MORTGAGE", "MORTGAGE_PROPERTY");
            }

            addButton("OK", "ACK");
        }
    }
    else if (status == "MORTGAGED")
    {
        addButton("REDEEM", "REDEEM_PROPERTY");
        addButton("OK", "ACK");
    }

    float startY = this->pos.y + this->getRenderDim().y / 2 - 60;

    for (int i = 0; i < actionButtons.size(); i++)
    {
        actionButtons[i].movePosition({this->pos.x,
                                       startY - i * 70});

        actionButtons[i].setRender([this, i]()
                                   {
            auto& btn = actionButtons[i];

            DrawRectangle(
                btn.getRenderPos().x,
                btn.getRenderPos().y,
                btn.getRenderWidth(),
                btn.getRenderHeight(),
                btn.getRenderColor(DARKGREEN)
            );

            std::string label = btn.getGameCommand();

            Vector2 t = MeasureTextEx(fontMap["Orbitron"],
                                    label.c_str(), 28, 1);

            DrawTextEx(fontMap["Orbitron"],
                    label.c_str(),
                    {btn.getPos().x - t.x / 2.0f, btn.getPos().y - t.y / 2.0f},
                    28, 1,
                    WHITE); });
    }
}

void PropertyPopup::enable()
{
    for (auto &btn : actionButtons)
        btn.enable();
    exitButton.enable();
}

void PropertyPopup::disable()
{
    for (auto &btn : actionButtons)
        btn.disable();
    exitButton.disable();
}

void PropertyPopup::addButton(const std::string &label, const std::string &command)
{
    Interactable btn(
        {220, 50},
        true,
        false,
        label,
        []() {},
        [this, command]()
        {
            this->actionCommand = command;
            this->closeView = true;
        });

    actionButtons.push_back(btn);
}

void PropertyPopup::interactionCheck()
{
    for (auto &btn : actionButtons)
        btn.interactionCheck();
    exitButton.interactionCheck();
}

std::string PropertyPopup::catchCommand()
{
    if (!actionCommand.empty())
        return actionCommand;

    for (auto &btn : actionButtons)
    {
        std::string cmd = btn.catchCommand();
        if (cmd != "NULL")
            return cmd;
    }

    return "NULL";
}

std::string PropertyPopup::buildDetails() const
{
    std::string s;

    s += "Status: " + status;
    if (status == "OWNED" && !ownerName.empty())
        s += " (" + ownerName + ")";
    s += "\n\n";

    s += "Type: " + type + "\n";

    if (type == "STREET")
    {
        s += "Color: " + colorGroup + "\n";

        if (status == "BANK")
        {
            s += "Buy Price: " + std::to_string(buyPrice) + "\n";
            return s;
        }

        if (status == "OWNED")
        {
            if (!rentTable.empty())
            {
                int idx = std::max(0, levelOrCount);
                idx = std::min(idx, (int)rentTable.size() - 1);
                int rent = rentTable[idx];
                s += "Current Rent: " + std::to_string(rent) + "\n";
            }

            if (!isOtherPlayer)
            {
                s += "Build Cost: " + std::to_string(buildCost) + "\n";
                s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
                s += "Level: " + std::to_string(levelOrCount) + "\n";
            }
        }

        if (status == "MORTGAGED")
        {
            s += "This property is mortgaged.\n";
            if (!isOtherPlayer)
            {
                s += "Mortgage Value: " + std::to_string(mortgageValue) + "\n";
            }
        }
    }
    else if (type == "RAILROAD")
    {
        if (status == "OWNED")
        {
            if (!railroadRent.empty())
            {
                int idx = std::max(0, levelOrCount - 1);
                idx = std::min(idx, (int)railroadRent.size() - 1);
                int rent = railroadRent[idx];
                s += "Rent: " + std::to_string(rent) + "\n";
            }

            s += "Owned Count: " + std::to_string(levelOrCount) + "\n";

            if (!isOtherPlayer)
            {
                s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
            }
        }
        else if (status == "MORTGAGED")
        {
            s += "This railroad is mortgaged.\n";

            if (!isOtherPlayer)
            {
                s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
            }
        }
    }
    else if (type == "UTILITY")
    {
        if (status == "OWNED")
        {
            if (!utilityMultiplier.empty())
            {
                int idx = std::max(0, levelOrCount - 1);
                idx = std::min(idx, (int)utilityMultiplier.size() - 1);
                int mult = utilityMultiplier[idx];
                s += "Multiplier: x" + std::to_string(mult) + "\n";
            }

            s += "Owned Count: " + std::to_string(levelOrCount) + "\n";

            if (!isOtherPlayer)
            {
                s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
            }
        }
        else if (status == "MORTGAGED")
        {
            s += "This utility is mortgaged.\n";

            if (!isOtherPlayer)
            {
                s += "Mortgage: " + std::to_string(mortgageValue) + "\n";
            }
        }
    }

    return s;
}

void PropertyPopup::render()
{
    animationCheck();

    DrawRectangle(getRenderPos().x, getRenderPos().y,
                  getRenderWidth(), getRenderHeight(),
                  RAYWHITE);

    DrawRectangle(getRenderPos().x, getRenderPos().y,
                  getRenderWidth(), getRenderHeight() * 0.125f,
                  RED);

    Vector2 t = MeasureTextEx(fontMap["Orbitron"], name.c_str(), 28, 1);
    DrawTextEx(fontMap["Orbitron"], name.c_str(),
               {pos.x - t.x / 2.0f, pos.y - (getRenderHeight() / 2.0f - 10.0f)},
               28, 1, WHITE);

    std::string text = buildDetails();

    std::stringstream ss(text);
    std::string line;
    std::string fixedText;

    while (std::getline(ss, line))
    {
        if (!line.empty())
        {
            fixedText += line + "\n";
        }
    }

    Vector2 wrapDim = {getRenderDim().x - 40.0f, getRenderDim().y};

    drawTextLinesWrapped(
        fontMap["Orbitron"],
        fixedText,
        pos,
        22,
        1,
        BLACK,
        wrapDim);

    for (auto &btn : actionButtons)
        btn.render();

    exitButton.render();
}
