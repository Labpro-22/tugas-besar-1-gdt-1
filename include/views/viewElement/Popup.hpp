#pragma once
#include "View2D.hpp"
#include "Interactable.hpp"

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
private:
    // Basic
    std::string name;
    std::string type;     // "STREET", "RAILROAD", "UTILITY"
    std::string status;   // "BANK", "OWNED", "MORTGAGED"

    // Common
    int buyPrice;
    int mortgageValue;

    int levelOrCount = 0;
    bool isOtherPlayer = false;

    std::string ownerName;

    // STREET
    std::string colorGroup;
    std::vector<int> rentTable;
    int buildCost;

    // RAILROAD & UTILITY
    std::vector<int> railroadRent;
    std::vector<int> utilityMultiplier;

    std::string actionCommand;
    std::vector<Interactable> actionButtons;

    // Helper
    std::string buildDetails() const;

public:
    PropertyPopup(
        const std::string& name,
        const std::string& type,
        const std::string& status,
        int buyPrice,
        int mortgageValue,

        int levelOrCount = 0,
        bool isOtherPlayer = false,

        const std::string& ownerName = "",
        const std::string& colorGroup = "",
        const std::vector<int>& rentTable = {},
        int buildCost = 0,
        const std::vector<int>& railroadRent = {},
        const std::vector<int>& utilityMultiplier = {}
    );

    void enable() override;
    void disable() override;
    void interactionCheck() override;
    void addButton(const std::string& label, const std::string& command);
    const string catchCommand() override;
    void render() override;
};