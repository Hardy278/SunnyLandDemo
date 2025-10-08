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
    void testCamera();
    void testPlayer();
    void testCollisionPairs();
};
}