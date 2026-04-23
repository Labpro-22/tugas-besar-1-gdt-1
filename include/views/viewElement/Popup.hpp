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
public:
    enum class Type {
        STREET,
        RAILROAD,
        UTILITY
    };

    enum class Status {
        BANK,
        OWNED,
        MORTGAGED
    };

private:
    // BASIC 
    std::string name;
    Type type;
    Status status;

    int buyPrice;
    int mortgageValue;

    bool isOtherPlayer = false;
    std::string ownerName;

    // STREET
    struct StreetData {
        std::string colorGroup;
        std::vector<int> rentTable;

        int baseRent = 0;        // penting untuk monopoli tanpa bangunan
        int buildCost = 0;
        int level = 0;           // 0–4 rumah, 5 = hotel
        bool colorGroupComplete = false;

        float festivalMultiplier = 1.0f; // optional future
    };

    // RAILROAD 
    struct RailroadData {
        std::vector<int> rentTable;
        int ownedCount = 0;
    };

    // UTILITY
    struct UtilityData {
        std::vector<int> multiplier;
        int ownedCount = 0;

        int lastDiceRoll = 0; // penting untuk hitung sewa
    };

    std::optional<StreetData> streetData;
    std::optional<RailroadData> railroadData;
    std::optional<UtilityData> utilityData;

    std::string actionCommand;
    std::vector<Interactable> actionButtons;

    // HELPER 
    std::string buildDetails() const;

public:
    PropertyPopup(
        const std::string& name,
        Type type,
        Status status,
        int buyPrice,
        int mortgageValue,
        bool isOtherPlayer = false,
        const std::string& ownerName = ""
    );

    ~PropertyPopup() noexcept override = default;

    // SETTER
    void setStreetData(
        const std::string& colorGroup,
        const std::vector<int>& rentTable,
        int baseRent,
        int buildCost,
        int level,
        bool colorGroupComplete
    );

    void setRailroadData(
        const std::vector<int>& rentTable,
        int ownedCount
    );

    void setUtilityData(
        const std::vector<int>& multiplier,
        int ownedCount,
        int diceRoll
    );

    void enable() override;
    void disable() override;
    void interactionCheck() override;

    void addButton(const std::string& label, const std::string& command);

    const std::string catchCommand() override;

    void render() override;
};