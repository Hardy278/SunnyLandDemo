#include "engine/core/Game.hpp"
#include <spdlog/spdlog.h>

int main() {
    try {
        spdlog::set_level(spdlog::level::trace);
        engine::core::Game game;
        game.run();
        return 0;
    } catch (const std::exception& e) {
        spdlog::error("程序异常终止: {}", e.what());
        return -1;
    }
}