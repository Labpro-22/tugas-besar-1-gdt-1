#include <iostream>
#include <exception>

#include "core/GameEngine.hpp"

// Entry point sementara. TODO:
//   1. Buat CLIRenderer (implementasi IGUI) dari tim UI → IGUI* gui = new CLIRenderer();
//   2. Oper ke GameEngine: GameEngine engine(gui);
//   3. Hapus gui setelah engine selesai (kepemilikan ada di main, bukan engine).
int main() {
    try {
        GameEngine engine;
        engine.run();
    } catch (const std::exception& e) {
        std::cerr << "[FATAL] " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
