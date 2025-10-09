#pragma once
#include "../../engine/scene/Scene.hpp"
#include <memory>

namespace engine::object {
    class GameObject;
}

namespace game::scene {
class GameScene : public engine::scene::Scene {
    engine::object::GameObject* m_player = nullptr;
public:
    GameScene(std::string name, engine::core::Context& context, engine::scene::SceneManager& sceneManager);

    void init() override;
    void update(float deltaTime) override;
    void render() override;
    void handleInput() override;
    void clean() override;

private:
    [[nodiscard]] bool initLevel();               ///< @brief 初始化关卡
    [[nodiscard]] bool initPlayer();              ///< @brief 初始化玩家
    [[nodiscard]] bool initEnemyAndItem();        ///< @brief 初始化敌人和道具
};
}