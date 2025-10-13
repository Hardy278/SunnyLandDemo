#include "engine/core/Game.hpp"
#include "engine/core/Context.hpp"
#include "engine/scene/SceneManager.hpp"
#include "game/scene/TitleScene.hpp"
#include <spdlog/spdlog.h>
#include <SDL3/SDL.h>

void setupInitialScene(engine::scene::SceneManager& sceneManager) {
    // GameApp在调用run方法之前，先创建并设置初始场景
    auto titleScene = std::make_unique<game::scene::TitleScene>(sceneManager.getContext(), sceneManager);
    sceneManager.requestPushScene(std::move(titleScene));
}

int main() {
    spdlog::set_level(spdlog::level::trace);

    engine::core::Game app;
    app.registerSceneSetup(setupInitialScene);
    app.run();
    return 0;
}