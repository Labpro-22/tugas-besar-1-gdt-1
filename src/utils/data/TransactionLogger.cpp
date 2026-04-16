#include "utils/data/TransactionLogger.hpp"

TransactionLogger::TransactionLogger() = default;

void TransactionLogger::log(int turn, std::string username, std::string actionType, std::string detail) {
    entries.emplace_back(turn, std::move(username), std::move(actionType), std::move(detail));
}

const std::vector<LogEntry>& TransactionLogger::getFullLog() const {
    return entries;
}

std::vector<LogEntry> TransactionLogger::getRecentLog(int n) const {
    if (n <= 0 || entries.empty()) {
        return {};
    }

    const int count = std::min<int>(n, static_cast<int>(entries.size()));
    const auto startIt = entries.end() - count;

    return std::vector<LogEntry>(startIt, entries.end());
}

int TransactionLogger::getEntryCount() const {
    return static_cast<int>(entries.size());
}

void TransactionLogger::clear() {
    entries.clear();
}

void TransactionLogger::loadEntries(std::vector<LogEntry> entries) {
    this->entries = std::move(entries);
}
