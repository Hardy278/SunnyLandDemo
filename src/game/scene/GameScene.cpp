#include "GameScene.hpp"
#include "../../engine/core/Context.hpp"
#include "../../engine/object/GameObject.hpp"
#include "../../engine/component/TransformComponent.hpp"
#include "../../engine/component/TileLayerComponent.hpp"
#include "../../engine/component/SpriteComponent.hpp"
#include "../../engine/component/ColliderComponent.hpp"
#include "../../engine/component/PhysicsComponent.hpp"
#include "../../engine/physics/PhysicsEngine.hpp"
#include "../../engine/physics/Collider.hpp"
#include "../../engine/scene/LevelLoader.hpp"
#include "../../engine/input/InputManager.hpp"
#include "../../engine/render/Camera.hpp"
#include "../../engine/utils/Math.hpp"
#include <spdlog/spdlog.h>
#include <glm/vec2.hpp>

namespace game::scene {
GameScene::GameScene(std::string name, engine::core::Context &context, engine::scene::SceneManager &sceneManager)
    : engine::scene::Scene(name, context, sceneManager) {
    spdlog::trace("GAMESCENE::构造完成");
}

void GameScene::init() {
    engine::scene::LevelLoader levelLoader;
    if (!levelLoader.loadLevel("assets/maps/level1.tmj", *this)) {
        spdlog::error("GameScene::init::加载关卡失败");
        return;
    }

    auto* mainLayer = findGameObjectByName("main");
    if (mainLayer) {
        auto* tileLayer = mainLayer->getComponent<engine::component::TileLayerComponent>();
        if (tileLayer) {
            m_context.getPhysicsEngine().registerCollisionLayer(tileLayer);
            spdlog::info("GAMESCENE::注册\"main\"层到物理引擎");
        }
    }

    m_player = findGameObjectByName("player");
    if (!m_player) {
        spdlog::error("GameScene::init::找不到玩家对象");
        return;
    }
    auto* playerTransform = m_player->getComponent<engine::component::TransformComponent>();
    if (playerTransform) {
        m_context.getCamera().setTarget(playerTransform);
    }
    // 设置相机边界
    auto worldSize = mainLayer->getComponent<engine::component::TileLayerComponent>()->getWorldSize();
    m_context.getCamera().setLimitBounds(engine::utils::Rect(glm::vec2(0.0f), worldSize));

    // 设置世界边界
    m_context.getPhysicsEngine().setWorldBounds(engine::utils::Rect(glm::vec2(0.0f), worldSize));

    Scene::init();
    spdlog::trace("GAMESCENE::初始化完成");
}

void GameScene::update(float deltaTime) {
    Scene::update(deltaTime);
}

void GameScene::render() {
    Scene::render();
}

void GameScene::handleInput() {
    Scene::handleInput();
    testPlayer();
    testCollisionPairs();
}

void GameScene::clean() {
    Scene::clean(); 
}

void GameScene::testCamera() {
    auto &camera = m_context.getCamera();
    auto &inputManager = m_context.getInputManager();
    if (inputManager.isActionDown("move_up")) camera.move(glm::vec2(0.0f, -2.0f));
    if (inputManager.isActionDown("move_down")) camera.move(glm::vec2(0.0f, 2.0f));
    if (inputManager.isActionDown("move_left")) camera.move(glm::vec2(-2.0f, 0.0f));
    if (inputManager.isActionDown("move_right")) camera.move(glm::vec2(2.0f, 0.0f));
}

void GameScene::testPlayer() {
    if (!m_player) return;
    auto &inputManager = m_context.getInputManager();
    auto *pc = m_player->getComponent<engine::component::PhysicsComponent>();
    if (!pc) return;
    if (inputManager.isActionDown("move_left")) {
        pc->m_velocity.x = -100.0f;
    } else {
        pc->m_velocity.x *= 0.9f;
    }
    if (inputManager.isActionDown("move_right")) {
        pc->m_velocity.x = 100.0f;
    } else {
        pc->m_velocity.x *= 0.9f;
    }
    if (inputManager.isActionDown("jump")) {
        pc->m_velocity.y = -300.0f;
    }
}

void GameScene::testCollisionPairs() {
    auto collisionPairs = m_context.getPhysicsEngine().getCollisionPairs();
    for (auto &pair : collisionPairs) {
        spdlog::trace("GAMESCENE::testCollisionPairs::碰撞对: {} - {}", pair.first->getName(), pair.second->getName());
    }
}

} // namespace game::scene