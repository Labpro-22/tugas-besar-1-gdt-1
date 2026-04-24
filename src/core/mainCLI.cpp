#include <iostream>
#include <exception>

#include "core/GameEngine.hpp"
#include "views/CLIGUI.hpp"

int main() {
    try {
        CLIGUI gui;
        GameEngine engine(&gui);
        engine.run();
    } catch (const std::exception& e) {
        std::cerr << "[FATAL] " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
