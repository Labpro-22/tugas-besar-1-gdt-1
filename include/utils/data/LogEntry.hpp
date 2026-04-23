#ifndef LOGENTRY_HPP
#define LOGENTRY_HPP

#include <string>
#include <iomanip>
#include <sstream>
#include <utility>

class LogEntry {
private:
    int turn;
    std::string username;
    std::string actionType;
    std::string detail;

public:
    LogEntry(int turn, std::string username, std::string actionType, std::string detail);

    int getTurn() const;
    std::string getUsername() const;
    std::string getActionType() const;
    std::string getDetail() const;

    std::string toString() const;

    std::string toSaveString() const;
};

#endif
