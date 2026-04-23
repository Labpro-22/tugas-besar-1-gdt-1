#ifndef TRANSACTIONLOGGER_HPP
#define TRANSACTIONLOGGER_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <utility>

#include "utils/data/LogEntry.hpp"

class TransactionLogger {
private:
    std::vector<LogEntry> entries;

public:
    TransactionLogger();

    void log(int turn, std::string username, std::string actionType, std::string detail);
    const std::vector<LogEntry>& getFullLog() const;
    std::vector<LogEntry> getRecentLog(int n) const;
    int getEntryCount() const;
    void clear();
    void loadEntries(std::vector<LogEntry> entries);
};

#endif
