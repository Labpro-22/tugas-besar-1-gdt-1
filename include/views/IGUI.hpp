#pragma once

#include <string>
#include <vector>

class Game;
class Player;
class Property;
class Tile;
class SkillCard;
class LogEntry;

class IGUI {
public:
    virtual ~IGUI() = default;

    // Lifecycle 
    virtual void update() = 0;
    virtual void display() = 0;
    virtual bool shouldExit() const = 0;

    // Input tunggal: command string dari user (kembalikan "NULL" / kosong jika belum ada)
    virtual std::string getCommand() = 0;

    // Navigasi view utama
    virtual void loadMainMenu() = 0;
    virtual void loadGameView() = 0;
    virtual void loadFinishMenu() = 0;

    // Popup / prompt
    virtual void showMessage(const std::string& message) = 0;
    virtual void showConfirm(const std::string& question) = 0;
    virtual void showInputPrompt(const std::string& prompt) = 0;
    virtual void showException(int code, const std::string& msg) = 0;
    virtual void waitForAnimationEnd() = 0;


    // Render state game
    virtual void renderBoard(const Game& game) = 0;
    virtual void renderPlayer(const Player& player) = 0;
    virtual void renderProperty(const Property& property) = 0;
    virtual void renderOwnedProperties(const Player& player) = 0;
    virtual void renderDice(int die1, int die2) = 0;
    virtual void renderLog(const std::vector<LogEntry>& entries, const std::string& title) = 0;
    virtual void renderSkillHand(const std::vector<SkillCard*>& hand) = 0;
    virtual void renderAuctionStart(Property* property, Player *auctioner, Game* game) = 0;
    virtual void renderAuction(int currentBid, const Player* highBidder) = 0;
    virtual void renderBankruptcy(const Player& player) = 0;
    virtual void renderWinner(const Player& winner) = 0;
    virtual void renderMovement(const std::string& playerName, int steps)= 0;
};
