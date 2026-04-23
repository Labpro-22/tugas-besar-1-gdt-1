#pragma once

#include <string>
#include <vector>
#include <iostream>

class Command
{
private:
    std::string type;
    std::vector<std::string> args;

public:
    Command() : type("NULL") {}

    Command(const std::string &type, const std::vector<std::string> &args)
        : type(type), args(args) {}

    static Command Null()
    {
        return Command("NULL", {});
    }

    const std::string &getType() const { return type; }
    const std::vector<std::string> &getArgs() const { return args; }

    bool isNull() const { return type == "NULL"; }

    // DEBUG PRINT
    void debugPrint() const
    {
        std::cout << "[COMMAND DEBUG]\n";
        std::cout << "Type: " << type << "\n";
        std::cout << "Args (" << args.size() << "): ";

        for (size_t i = 0; i < args.size(); ++i)
        {
            std::cout << "[" << i << "]=\"" << args[i] << "\" ";
        }
        std::cout << "\n";
    }
};