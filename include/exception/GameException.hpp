#pragma once
#include <exception>
#include <string>

class GameException : public std::exception {
protected:
    int errorCode;
    std::string message;
public:
    explicit GameException(const std::string& msg);
    const char* what() const noexcept override;
};