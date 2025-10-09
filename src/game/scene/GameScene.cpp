#include "GameScene.hpp"
#include "../component/PlayerComponent.hpp"
#include "../../engine/core/Context.hpp"
#include "../../engine/object/GameObject.hpp"
#include "../../engine/component/TransformComponent.hpp"
#include "../../engine/component/TileLayerComponent.hpp"
#include "../../engine/component/AnimationComponent.hpp"
#include "../../engine/component/SpriteComponent.hpp"
#include "../../engine/component/ColliderComponent.hpp"
#include "../../engine/component/HealthComponent.hpp"
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
    if (m_isInitialized) {
        spdlog::warn("GAMESCENE::init::ERROR::GameScene 已经初始化过了，重复调用 init()。");
        return;
    }
    spdlog::trace("GAMESCENE::init::TRACE::GameScene 初始化开始...");

    if (!initLevel()) {
        spdlog::error("GAMESCENE::init::ERROR::关卡初始化失败，无法继续。");
        m_context.getInputManager().setShouldQuit(true);
        return;
    }
    if (!initPlayer()) {
        spdlog::error("GAMESCENE::init::ERROR::玩家初始化失败，无法继续。");
        m_context.getInputManager().setShouldQuit(true);
        return;
    }
    if (!initEnemyAndItem()) {
        spdlog::error("GAMESCENE::init::ERROR::敌人及道具初始化失败，无法继续。");
        m_context.getInputManager().setShouldQuit(true);
        return;
    }
    Scene::init();
    spdlog::trace("GAMESCENE::init::TRACE::GameScene 初始化完成。");
}

void GameScene::update(float deltaTime) {
    Scene::update(deltaTime);
}

void GameScene::render() {
    Scene::render();
}

void GameScene::handleInput() {
    Scene::handleInput();
}

void GameScene::clean() {
    Scene::clean(); 
}

bool GameScene::initLevel() {
    // 加载关卡（levelLoader通常加载完成后即可销毁，因此不存为成员变量）
    engine::scene::LevelLoader levelLoader;
    if (!levelLoader.loadLevel("assets/maps/level1.tmj", *this)){
        spdlog::error("GAMESCENE::initLevel::ERROR::关卡加载失败");
        return false;
    }

    // 注册"main"层到物理引擎
    auto* mainLayer = findGameObjectByName("main");
    if (!mainLayer) {
        spdlog::error("GAMESCENE::initLevel::ERROR::未找到\"main\"层");
        return false;
    }
    auto* tileLayer = mainLayer->getComponent<engine::component::TileLayerComponent>();
    if (!tileLayer) {
        spdlog::error("GAMESCENE::initLevel::ERROR::\"main\"层没有 TileLayerComponent 组件");
        return false;
    }
    m_context.getPhysicsEngine().registerCollisionLayer(tileLayer);
    spdlog::info("GAMESCENE::initLevel::INFO::注册\"main\"层到物理引擎");
    
    // 设置相机边界
    auto worldSize = mainLayer->getComponent<engine::component::TileLayerComponent>()->getWorldSize();
    m_context.getCamera().setLimitBounds(engine::utils::Rect(glm::vec2(0.0f), worldSize));
    m_context.getCamera().setPosition(glm::vec2(0.0f));     // 开始时重置相机位置，以免切换场景时晃动

    // 设置世界边界
    m_context.getPhysicsEngine().setWorldBounds(engine::utils::Rect(glm::vec2(0.0f), worldSize));

    spdlog::trace("GAMESCENE::initLevel::TRACE::关卡初始化完成。");
    return true;
}

bool GameScene::initPlayer() {
    // 获取玩家对象
    m_player = findGameObjectByName("player");
    if (!m_player) {
        spdlog::error("GAMESCENE::initPlayer::ERROR::未找到玩家对象");
        return false;
    }

    // 添加PlayerComponent到玩家对象
    auto* playerComponent = m_player->addComponent<game::component::PlayerComponent>();
    if (!playerComponent) {
        spdlog::error("GAMESCENE::initPlayer::ERROR::无法添加 PlayerComponent 到玩家对象");
        return false;
    }

    // 相机跟随玩家
    auto* m_playertransform = m_player->getComponent<engine::component::TransformComponent>();
    if (!m_playertransform) {
        spdlog::error("GAMESCENE::initPlayer::ERROR::玩家对象没有 TransformComponent 组件, 无法设置相机目标");
        return false;
    }
    m_context.getCamera().setTarget(m_playertransform);
    spdlog::trace("GAMESCENE::initPlayer::TRACE::Player初始化完成。");
    return true;
}

bool GameScene::initEnemyAndItem() {
    bool success = true;
    for (auto& game_object : m_gameObjects){
        if (game_object->getName() == "eagle"){
            if (auto ac = game_object->getComponent<engine::component::AnimationComponent>(); ac){
                ac->playAnimation("fly");
            } else {
                spdlog::error("GAMESCENE::initEnemyAndItem::ERROR::eagle 没有AnimationComponent组件, 无法播放动画");
                success = false;
            }
        }
        if (game_object->getName() == "frog"){
            if (auto ac = game_object->getComponent<engine::component::AnimationComponent>(); ac){
                ac->playAnimation("idle");
            } else {
                spdlog::error("GAMESCENE::initEnemyAndItem::ERROR::frog 没有AnimationComponent组件, 无法播放动画");
                success = false;
            }
        }
        if (game_object->getName() == "opossum"){
            if (auto ac = game_object->getComponent<engine::component::AnimationComponent>(); ac){
                ac->playAnimation("walk");
            } else {
                spdlog::error("GAMESCENE::initEnemyAndItem::ERROR::opossum 没有AnimationComponent组件, 无法播放动画");
                success = false;
            }
        }
        if (game_object->getTag() == "item"){
            if (auto ac = game_object->getComponent<engine::component::AnimationComponent>(); ac){
                ac->playAnimation("idle");
            } else {
                spdlog::error("GAMESCENE::initEnemyAndItem::ERROR::item 没有AnimationComponent组件, 无法播放动画");
                success = false;
            }
        }
    }
    return success;
}

} // namespace game::scene