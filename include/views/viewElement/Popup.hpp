#pragma once
#include "View2D.hpp"
#include "Interactable.hpp"
#include "views/viewElement/Entry.hpp"

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

class MessagePopup : public IndefinitePopup {
private:
    std::string message;
    Interactable okButton;

public:
    MessagePopup(const std::string& msg);

    void enable() override;
    void disable() override;
    void interactionCheck() override;
    std::string catchCommand() override;
    void render() override;
};

class InputPopup : public IndefinitePopup {
private:
    std::string title;
    Entry inputEntry;
    Interactable submitButton;

public:
    InputPopup(const std::string& title);

    void enable() override;
    void disable() override;
    void interactionCheck() override;
    std::string catchCommand() override;
    void render() override;
};

class ConfirmPopup : public IndefinitePopup {
private:
    std::string question;
    Interactable yesButton;
    Interactable noButton;

public:
    ConfirmPopup(const std::string& question);

    void enable() override;
    void disable() override;
    void interactionCheck() override;
    std::string catchCommand() override;
    void render() override;
};

class LoadConfirmPopup : public IndefinitePopup {
private:
    std::string title;
    Entry entry;
    Interactable confirmButton;
    std::function<void(const std::string&)> onSubmit;

public:
    LoadConfirmPopup(const std::string& title, const std::string& placeholder);

    void setOnSubmit(std::function<void(const std::string&)> func);

    void enable() override;
    void disable() override;
    void interactionCheck() override;
    std::string catchCommand() override;
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
    string catchCommand() override;
    void render() override;
};