#include "GameScene.hpp"
#include "MenuScene.hpp"
#include "EndScene.hpp"
#include "../component/PlayerComponent.hpp"
#include "../component/AIComponent.hpp"
#include "../component/AI/PatrolBehavior.hpp"
#include "../component/AI/UpdownBehavior.hpp"
#include "../component/AI/JumpBehavior.hpp"
#include "../data/SessionData.hpp"
#include "../../engine/core/Context.hpp"
#include "../../engine/core/GameState.hpp"
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
#include "../../engine/scene/SceneManager.hpp"
#include "../../engine/input/InputManager.hpp"
#include "../../engine/render/Camera.hpp"
#include "../../engine/render/Animation.hpp"
#include "../../engine/render/TextRenderer.hpp"
#include "../../engine/UI/UIManager.hpp"
#include "../../engine/UI/UIPanel.hpp"
#include "../../engine/UI/UILabel.hpp"
#include "../../engine/UI/UIImage.hpp"
#include "../../engine/utils/Math.hpp"
#include <spdlog/spdlog.h>
#include <glm/vec2.hpp>
#include <string_view>

namespace game::scene {

GameScene::GameScene( engine::core::Context& context, engine::scene::SceneManager& scene_manager,std::shared_ptr<game::data::SessionData> data)
    : Scene("GameScene", context, scene_manager), m_gameSessionData(std::move(data)) {
    if (!m_gameSessionData) {      // 如果没有传入SessionData，则创建一个默认的
        m_gameSessionData = std::make_shared<game::data::SessionData>();
        spdlog::info("未提供 SessionData, 使用默认值。");
    }
    spdlog::trace("GameScene 构造完成。");
}

void GameScene::init() {
    if (m_isInitialized) {
        spdlog::warn("GAMESCENE::init::ERROR::GameScene 已经初始化过了，重复调用 init()。");
        return;
    }
    spdlog::trace("GAMESCENE::init::TRACE::GameScene 初始化开始...");
    m_context.getGameState().setState(engine::core::State::Playing);
    m_gameSessionData->syncHighScore("assets/save.json");      // 更新最高分

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
    if (!initUI()) {
        spdlog::error("GAMESCENE::init::ERROR::UI初始化失败, 无法继续。");
        m_context.getInputManager().setShouldQuit(true);
        return;
    }
    Scene::init();
    spdlog::trace("GAMESCENE::init::TRACE::GameScene 初始化完成。");
}

void GameScene::update(float deltaTime) {
    Scene::update(deltaTime);
    handleObjectCollisions(); // 处理对象碰撞
    handleTileTriggers();    // 处理触发器

    // 玩家掉出地图下方则判断为失败
    if (m_player ) {
        auto pos = m_player->getComponent<engine::component::TransformComponent>()->getPosition();
        auto worldRect = m_context.getPhysicsEngine().getWorldBounds();
        // 多100像素冗余量
        if (worldRect && pos.y > worldRect->position.y + worldRect->size.y + 100.0f) {
            spdlog::debug("玩家掉出地图下方，游戏失败");
            showEndScene(false);
        }
    }
}

void GameScene::render() {
    Scene::render();
}

void GameScene::handleInput() {
    Scene::handleInput();
    // 检查暂停动作
    if (m_context.getInputManager().isActionPressed("pause")) {
        spdlog::debug("在GameScene中检测到暂停动作，正在推送MenuScene。");
        m_sceneManager.requestPushScene(std::make_unique<MenuScene>(m_context, m_sceneManager, m_gameSessionData));
    }
}

void GameScene::clean() {
    Scene::clean(); 
}

bool GameScene::initLevel() {
    // 加载关卡（levelLoader通常加载完成后即可销毁，因此不存为成员变量）
    engine::scene::LevelLoader levelLoader;
    auto levelPath = m_gameSessionData->getMapPath();
    if (!levelLoader.loadLevel(levelPath, *this)){
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

    // 从SessionData中更新玩家生命值
    if (auto healthComponent = m_player->getComponent<engine::component::HealthComponent>(); healthComponent) {
        healthComponent->setMaxHealth(m_gameSessionData->getMaxHealth());
        healthComponent->setCurrentHealth(m_gameSessionData->getCurrentHealth());
    } else {
        spdlog::error("玩家对象缺少 HealthComponent 组件，无法设置生命值");
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
    for (auto& gameObject : m_gameObjects){
        if (gameObject->getName() == "eagle"){
            if (auto* AIComponent = gameObject->addComponent<game::component::AIComponent>(); AIComponent){
                auto yMax = gameObject->getComponent<engine::component::TransformComponent>()->getPosition().y;
                auto yMin = yMax - 80.0f;    // 让鹰的飞行范围 (当前位置与上方80像素 的区域)
                AIComponent->setBehavior(std::make_unique<game::component::ai::UpDownBehavior>(yMin, yMax));
            }
        }
        if (gameObject->getName() == "frog"){
            if (auto* AIComponent = gameObject->addComponent<game::component::AIComponent>(); AIComponent){
                auto xMax = gameObject->getComponent<engine::component::TransformComponent>()->getPosition().x - 10.0f;
                auto xMin = xMax - 90.0f;    // 青蛙跳跃范围（右侧 - 10.0f 是为了增加稳定性）
                AIComponent->setBehavior(std::make_unique<game::component::ai::JumpBehavior>(xMin, xMax));
            }
        }
        if (gameObject->getName() == "opossum"){
            if (auto* AIComponent = gameObject->addComponent<game::component::AIComponent>(); AIComponent){
                auto xMax = gameObject->getComponent<engine::component::TransformComponent>()->getPosition().x;
                auto xMin = xMax - 200.0f;    // 负鼠巡逻范围
                AIComponent->setBehavior(std::make_unique<game::component::ai::PatrolBehavior>(xMin, xMax));
            }
        }
        if (gameObject->getTag() == "item"){
            if (auto* ac = gameObject->getComponent<engine::component::AnimationComponent>(); ac){
                ac->playAnimation("idle");
            } else {
                spdlog::error("Item对象缺少 AnimationComponent, 无法播放动画。");
                success = false;
            }
        }
    }
    return success;
}

bool GameScene::initUI() {
    if (!m_UIManager->init(m_context.getGameState().getLogicalSize())) return false;
    createScoreUI();
    createHealthUI();
    return true;
}

void GameScene::handleObjectCollisions() {
    // 从物理引擎中获取碰撞对
    auto collisionPairs = m_context.getPhysicsEngine().getCollisionPairs();
    for (const auto& pair : collisionPairs) {
        auto* obj1 = pair.first;
        auto* obj2 = pair.second;
        // 处理玩家与敌人的碰撞
        if (obj1->getName() == "player" && obj2->getTag() == "enemy") {
            playerVSEnemyCollision(obj1, obj2);
        } else if (obj2->getName() == "player" && obj1->getTag() == "enemy") {
            playerVSEnemyCollision(obj2, obj1);
        }
        // 处理玩家与道具的碰撞
        else if (obj1->getName() == "player" && obj2->getTag() == "item") {
            playerVSItemCollision(obj1, obj2);
        } else if (obj2->getName() == "player" && obj1->getTag() == "item") {
            playerVSItemCollision(obj2, obj1);
        }
        // 处理玩家与"hazard"对象碰撞
        else if (obj1->getName() == "player" && obj2->getTag() == "hazard") {
            m_player->getComponent<game::component::PlayerComponent>()->takeDamage(1);
            spdlog::debug("玩家 {} 受到了 HAZARD 对象伤害", obj1->getName());
        } else if (obj2->getName() == "player" && obj1->getTag() == "hazard") {
            m_player->getComponent<game::component::PlayerComponent>()->takeDamage(1);
            spdlog::debug("玩家 {} 受到了 HAZARD 对象伤害", obj2->getName());
        }
        // 处理玩家与关底触发器碰撞
        else if (obj1->getName() == "player" && obj2->getTag() == "next_level") {
            toNextLevel(obj2);
        } else if (obj2->getName() == "player" && obj1->getTag() == "next_level") {
            toNextLevel(obj1);
        }
        // 处理玩家与结束触发器碰撞
        else if (obj1->getName() == "player" && obj2->getName() == "win") {
            showEndScene(true);
        } else if (obj2->getName() == "player" && obj1->getName() == "win") {
            showEndScene(true);
        }
    }
}

void GameScene::handleTileTriggers() {
    const auto& tileTriggerEvents = m_context.getPhysicsEngine().getTileTriggerEvents();
    for (const auto& event : tileTriggerEvents) {
        auto* obj = event.first;        // 瓦片触发事件的对象
        auto tileType = event.second;  // 瓦片类型
        if (tileType == engine::component::TileType::HAZARD) {
            // 玩家碰到到危险瓦片，受伤
            if (obj->getName() == "player") {
                handlePlayerDamage(1);
                spdlog::debug("玩家 {} 受到了 HAZARD 瓦片伤害", obj->getName());
            } 
            // TODO: 其他对象类型的处理，目前让敌人无视瓦片伤害
        }
    }
}

void GameScene::handlePlayerDamage(int damage) {
    auto playerComponent = m_player->getComponent<game::component::PlayerComponent>();
    if (!playerComponent->takeDamage(damage)) { // 没有受伤，直接返回
        return;
    }
    if (playerComponent->isDead()) {
        spdlog::info("玩家 {} 死亡", m_player->getName());
        // TODO: 可能的死亡逻辑处理
    }
    // 更新生命值及HealthUI
    updateHealthWithUI();
}

void GameScene::playerVSEnemyCollision(engine::object::GameObject *player, engine::object::GameObject *enemy) {
    // --- 踩踏判断逻辑：1. 玩家中心点在敌人上方    2. 重叠区域：overlap.x > overlap.y
    auto playerAABB = player->getComponent<engine::component::ColliderComponent>()->getWorldAABB();
    auto enemyAABB = enemy->getComponent<engine::component::ColliderComponent>()->getWorldAABB();
    auto playerCenter = playerAABB.position + playerAABB.size / 2.0f;
    auto enemyCenter = enemyAABB.position + enemyAABB.size / 2.0f;
    auto overlap = glm::vec2(playerAABB.size / 2.0f + enemyAABB.size / 2.0f) - glm::abs(playerCenter - enemyCenter);

    // 踩踏判断成功，敌人受伤
    if (overlap.x > overlap.y && playerCenter.y < enemyCenter.y) {    
        spdlog::info("GAMESCENE::playerVSEnemyCollision::INFO::玩家 {} 踩踏了敌人 {}", player->getName(), enemy->getName());
        auto enemyHealth = enemy->getComponent<engine::component::HealthComponent>();
        if (!enemyHealth) {
            spdlog::error("GAMESCENE::playerVSEnemyCollision::ERROR::敌人 {} 没有 HealthComponent 组件，无法处理踩踏伤害", enemy->getName());
            return;
        }
        enemyHealth->takeDamage(1);  // 造成1点伤害
        if (!enemyHealth->isAlive()) {
            spdlog::info("GAMESCENE::playerVSEnemyCollision::INFO::敌人 {} 被踩踏后死亡", enemy->getName());
            enemy->setNeedRemove(true);  // 标记敌人为待删除状态
            createEffect(enemyCenter, enemy->getTag());  // 创建（死亡）特效
        }
        // 玩家跳起效果
        player->getComponent<engine::component::PhysicsComponent>()->m_velocity.y = -300.0f;  // 向上跳起
        // 加分
        addScoreWithUI(1);
    } else {// 踩踏判断失败，玩家受伤
        spdlog::info("GAMESCENE::playerVSEnemyCollision::INFO::敌人 {} 对玩家 {} 造成伤害", enemy->getName(), player->getName());
        handlePlayerDamage(1);  // 造成1点伤害
    }
}

void GameScene::playerVSItemCollision(engine::object::GameObject*, engine::object::GameObject * item) {
    if (item->getName() == "fruit") {
        healWithUI(1);        // 加血
    } else if (item->getName() == "gem") {
        addScoreWithUI(5);    // 加5分
    }
    item->setNeedRemove(true);  // 标记道具为待删除状态
    auto itemAABB = item->getComponent<engine::component::ColliderComponent>()->getWorldAABB();
    createEffect(itemAABB.position + itemAABB.size / 2.0f, item->getTag());  // 创建特效
}

void GameScene::toNextLevel(engine::object::GameObject *trigger) {
    auto sceneName = trigger->getName();
    auto mapPath = levelNameToPath(sceneName);
    m_gameSessionData->setNextLevel(mapPath);     // 设置下一个关卡信息
    auto nextScene = std::make_unique<game::scene::GameScene>(m_context, m_sceneManager, m_gameSessionData);
    m_sceneManager.requestReplaceScene(std::move(nextScene));
}

void GameScene::showEndScene(bool isWin) {
    spdlog::debug("显示结束场景，游戏 {}", isWin ? "胜利" : "失败");
    m_gameSessionData->setIsWin(isWin);
    auto end_scene = std::make_unique<game::scene::EndScene>(m_context, m_sceneManager, m_gameSessionData);
    m_sceneManager.requestPushScene(std::move(end_scene));
}

void GameScene::createEffect(glm::vec2 centerPos, std::string_view tag) {
    // --- 创建游戏对象和变换组件 ---
    auto effectObj = std::make_unique<engine::object::GameObject>("effect_" + std::string(tag));
    effectObj->addComponent<engine::component::TransformComponent>(std::move(centerPos));

    // --- 根据标签创建不同的精灵组件和动画--- 
    auto animation = std::make_unique<engine::render::Animation>("effect", false);
    if (tag == "enemy") {
        effectObj->addComponent<engine::component::SpriteComponent>("assets/textures/FX/enemy-deadth.png", m_context.getResourceManager(), engine::utils::Alignment::CENTER);
        for (auto i = 0; i < 5; ++i) {
            animation->addFrame({static_cast<float>(i * 40), 0.0f, 40.0f, 41.0f}, 0.1f);
        }
    } else if (tag == "item") {
        effectObj->addComponent<engine::component::SpriteComponent>("assets/textures/FX/item-feedback.png", m_context.getResourceManager(), engine::utils::Alignment::CENTER);
        for (auto i = 0; i < 4; ++i) {
            animation->addFrame({static_cast<float>(i * 32), 0.0f, 32.0f, 32.0f}, 0.1f);
        }
    } else {
        spdlog::warn("GAMESCENE::createEffect::WARN::未知特效类型: {}", tag);
        return;
    }

    // --- 根据创建的动画，添加动画组件，并设置为单次播放 ---
    auto* animationComponent = effectObj->addComponent<engine::component::AnimationComponent>();
    animationComponent->addAnimation(std::move(animation));
    animationComponent->setOneShotRemoval(true);
    animationComponent->playAnimation("effect");
    safeAddGameObject(std::move(effectObj));  // 安全添加特效对象
    spdlog::debug("创建特效: {}", tag);
}

void GameScene::createScoreUI() {
    auto scoreText = "Score: " + std::to_string(m_gameSessionData->getCurrentScore());    // 创建得分标签
    auto scoreLabel = std::make_unique<engine::ui::UILabel>(m_context.getTextRenderer(), scoreText, "assets/fonts/VonwaonBitmap-16px.ttf", 16);
    m_scoreLabel = scoreLabel.get();           // 成员变量赋值（获取裸指针）
    auto screenSize = m_UIManager->getRootElement()->getSize();        // 获取屏幕尺寸
    m_scoreLabel->setPosition(glm::vec2(screenSize.x - 100.0f, 10.0f));
    m_UIManager->addElement(std::move(scoreLabel));
}

void GameScene::createHealthUI() {
    int maxHealth = m_gameSessionData->getMaxHealth();
    int currentHealth = m_gameSessionData->getCurrentHealth();
    float startX = 10.0f;
    float startY = 10.0f;
    float iconWidth = 20.0f;
    float iconHeight = 18.0f;
    float spacing = 5.0f;
    std::string fullHeartTex = "assets/textures/UI/Heart.png";
    std::string emptyHeartTex = "assets/textures/UI/Heart-bg.png";

    // 创建一个默认的UIPanel (不需要背景色，因此大小无所谓，只用于定位)
    auto healthPanel = std::make_unique<engine::ui::UIPanel>();   
    m_healthPanel = healthPanel.get();           // 成员变量赋值（获取裸指针）

    // --- 根据最大生命值，循环创建生命值图标(添加到UIPanel中) ---
    for (int i = 0; i < maxHealth; ++i) {          // 创建背景图标
        glm::vec2 iconPos = {startX + i * (iconWidth + spacing), startY};
        glm::vec2 iconSize = {iconWidth, iconHeight};

        auto bgIcon = std::make_unique<engine::ui::UIImage>(emptyHeartTex, iconPos, iconSize);
        m_healthPanel->addChild(std::move(bgIcon));
    }
    for (int i = 0; i < maxHealth; ++i) {          // 创建前景图标
        glm::vec2 iconPos = {startX + i * (iconWidth + spacing), startY};
        glm::vec2 iconSize = {iconWidth, iconHeight};

        auto fgIcon = std::make_unique<engine::ui::UIImage>(fullHeartTex, iconPos, iconSize);
        bool isVisible = (i < currentHealth);  // 前景图标的可见性取决于当前生命值
        fgIcon->setVisible(isVisible);         // 设置前景图标的可见性
        m_healthPanel->addChild(std::move(fgIcon));
    }
    // 将UIPanel添加到UI管理器中
    m_UIManager->addElement(std::move(healthPanel));
}

void GameScene::updateHealthWithUI() {
    if (!m_player || !m_healthPanel) {
        spdlog::error("玩家对象或 HealthPanel 不存在, 无法更新生命值UI");
        return;
    }
    // 获取当前生命值并更新游戏数据
    auto currentHealth = m_player->getComponent<engine::component::HealthComponent>()->getCurrentHealth();
    m_gameSessionData->setCurrentHealth(currentHealth);
    auto maxHealth = m_gameSessionData->getMaxHealth();

    // 前景图标是后添加的，因此设置后半段的可见性即可
    for (auto i = maxHealth; i < maxHealth * 2; ++i) {
        m_healthPanel->getChildren()[i]->setVisible(i - maxHealth < currentHealth);
    }
}

void GameScene::addScoreWithUI(int score) {
    m_gameSessionData->addScore(score);
    auto scoreText = "Score: " + std::to_string(m_gameSessionData->getCurrentScore());
    spdlog::info("得分: {}", scoreText);
    m_scoreLabel->setText(scoreText);
}

void GameScene::healWithUI(int amount) {
    m_player->getComponent<engine::component::HealthComponent>()->heal(amount);
    updateHealthWithUI();                              // 更新生命值与UI
}

} // namespace game::scene