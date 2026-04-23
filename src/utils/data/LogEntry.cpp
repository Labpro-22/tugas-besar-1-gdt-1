#include "utils/data/LogEntry.hpp"

LogEntry::LogEntry(int turn, std::string username, std::string actionType, std::string detail): turn(turn), username(std::move(username)),
actionType(std::move(actionType)),detail(std::move(detail)) {}

int LogEntry::getTurn() const {
    return turn;
}

std::string LogEntry::getUsername() const {
    return username;
}

std::string LogEntry::getActionType() const {
    return actionType;
}

std::string LogEntry::getDetail() const {
    return detail;
}

std::string LogEntry::toString() const {
    std::ostringstream oss;
    oss << "[Turn " << turn << "] "
        << username << " | "
        << std::left << std::setw(9) << actionType << " | "
        << detail;
    return oss.str();
}

std::string LogEntry::toSaveString() const {
    std::ostringstream oss;
    oss << turn << ' ' << username << ' ' << actionType << ' ' << detail;
    return oss.str();
}
