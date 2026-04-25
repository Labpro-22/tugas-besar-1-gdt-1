#pragma once

#include <string>

class Formatter {
public:
    static std::string money(int amount) {
        const bool negative = amount < 0;
        long long absAmount = negative ? -static_cast<long long>(amount) : amount;
        std::string digits = std::to_string(absAmount);
        std::string grouped;
        int count = 0;

        for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
            grouped.insert(grouped.begin(), digits[i]);
            if (++count == 3 && i > 0) {
                grouped.insert(grouped.begin(), '.');
                count = 0;
            }
        }

        return std::string(negative ? "-M" : "M") + grouped;
    }
};
