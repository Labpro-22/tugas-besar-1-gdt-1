#pragma once
#include "View2D.hpp"
#include "Interactable.hpp"
#include "models/Property/Property.hpp"
#include "models/Property/StreetProperty.hpp"
#include "models/Property/RailroadProperty.hpp"
#include "models/Property/UtilityProperty.hpp"
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
    Property* property;
    bool isOtherPlayer;

    std::string actionCommand;
    std::vector<Interactable> actionButtons;

    virtual std::string buildDetails() const = 0;

public:
    PropertyPopup(
        Property* property,
        bool isOtherPlayer = false
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
    bool colorGroupComplete;

public:
    StreetPopup(
        StreetProperty* street,
        bool isOtherPlayer = false,
        bool colorGroupComplete = false
    );

protected:
    std::string buildDetails() const override;
};

class RailroadPopup : public PropertyPopup {
private:
    int ownedCount;

public:
    RailroadPopup(
        RailroadProperty* railroad,
        bool isOtherPlayer = false,
        int ownedCount = 0
    );

protected:
    std::string buildDetails() const override;
};

class UtilityPopup : public PropertyPopup {
private:
    int ownedCount;
    int lastDiceRoll;

public:
    UtilityPopup(
        UtilityProperty* utility,
        bool isOtherPlayer = false,
        int ownedCount = 0,
        int diceRoll = 0
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