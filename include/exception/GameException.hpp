#pragma once
#include <exception>
#include <string>

class GameException : public std::exception
{
protected:
    int errorCode;
    std::string errorMessage;

public:
    explicit GameException(int errorCode, const std::string &errorMessage);
    virtual int getErrorCode() const;
    virtual const char *what() const noexcept override;
};