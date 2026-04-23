#pragma once
#include "View2D.hpp"
#include "Interactable.hpp"
#include <optional>

class Popup : public View2D {
    public :
        Popup(const View2D& view);
};

class TimedPopup : public Popup {
    protected:
        float popupDuration;
    public :
        TimedPopup(const View2D& view, float duration);
};

class IndefinitePopup : public Popup {
    protected:
        Interactable exitButton;
    public :
        IndefinitePopup(const View2D& view);
};

class LoadConfirmPopup : public IndefinitePopup {
    private:
        string filePath;
        Interactable confirmButton;
    public:
        LoadConfirmPopup(const string filePath);
        void enable() override;
        void disable() override;
        void interactionCheck() override;
        const string catchCommand() override;
        void render() override;
};

class ExceptionPopup : public IndefinitePopup
{
private:
    int errorCode;
    std::string errorMessage;
    Interactable okButton;

public:
    ExceptionPopup(int errorCode, const std::string &errorMessage);
    void enable() override;
    void disable() override;
    void interactionCheck() override;
    const string catchCommand() override;
    void render() override;
};

class PropertyPopup : public IndefinitePopup {
protected:
    std::string name;
    std::string ownerName;
    bool isOtherPlayer;

    int buyPrice;
    int mortgageValue;

    std::string actionCommand;
    std::vector<Interactable> actionButtons;

    virtual std::string buildDetails() const = 0;

public:
    PropertyPopup(
        const std::string& name,
        int buyPrice,
        int mortgageValue,
        bool isOtherPlayer = false,
        const std::string& ownerName = ""
    );

    virtual ~PropertyPopup() = default;
    void addButton(const std::string& label, const std::string& command);

    const std::string catchCommand() override;

    void enable() override;
    void disable() override;
    void interactionCheck() override;

    void render() override;
};

class StreetPopup : public PropertyPopup {
private:
    std::string colorGroup;
    std::vector<int> rentTable;

    int baseRent;
    int buildCost;
    int level;
    bool colorGroupComplete;

public:
    StreetPopup(
        const std::string& name,
        int buyPrice,
        int mortgageValue,
        const std::string& colorGroup,
        const std::vector<int>& rentTable,
        int baseRent,
        int buildCost,
        int level,
        bool colorGroupComplete,
        bool isOtherPlayer = false,
        const std::string& ownerName = ""
    );

protected:
    std::string buildDetails() const override;
};

class RailroadPopup : public PropertyPopup {
private:
    std::vector<int> rentTable;
    int ownedCount;

public:
    RailroadPopup(
        const std::string& name,
        int buyPrice,
        int mortgageValue,
        const std::vector<int>& rentTable,
        int ownedCount,
        bool isOtherPlayer = false,
        const std::string& ownerName = ""
    );

protected:
    std::string buildDetails() const override;
};

class UtilityPopup : public PropertyPopup {
private:
    std::vector<int> multiplier;
    int ownedCount;
    int lastDiceRoll;

public:
    UtilityPopup(
        const std::string& name,
        int buyPrice,
        int mortgageValue,
        const std::vector<int>& multiplier,
        int ownedCount,
        int diceRoll,
        bool isOtherPlayer = false,
        const std::string& ownerName = ""
    );

protected:
    std::string buildDetails() const override;
};

class MessagePopup : public IndefinitePopup {
private:
    std::string title;
    std::string message;

    // optional image
    Texture2D texture;
    bool hasImage;
    Vector2 imageSize;

public:
    MessagePopup(
        const std::string& title,
        const std::string& message,
        const std::string& imagePath = ""
    );

    ~MessagePopup() override;

    void enable() override;
    void disable() override;
    void interactionCheck() override;
    void render() override;
};