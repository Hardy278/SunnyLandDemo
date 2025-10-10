#pragma once
#include "../../engine/scene/Scene.hpp"
#include <glm/vec2.hpp>
#include <memory>

namespace engine::object {
    class GameObject;
}

namespace game::scene {
class GameScene : public engine::scene::Scene {
    engine::object::GameObject* m_player = nullptr;
public:
    GameScene(std::string_view name, engine::core::Context& context, engine::scene::SceneManager& sceneManager);

    void init() override;
    void update(float deltaTime) override;
    void render() override;
    void handleInput() override;
    void clean() override;

private:
    [[nodiscard]] bool initLevel();               ///< @brief 初始化关卡
    [[nodiscard]] bool initPlayer();              ///< @brief 初始化玩家
    [[nodiscard]] bool initEnemyAndItem();        ///< @brief 初始化敌人和道具
    [[nodiscard]] bool initUI();                  ///< @brief 初始化UI

    void handleObjectCollisions();              ///< @brief 处理游戏对象间的碰撞逻辑（从PhysicsEngine获取信息）
    void handleTileTriggers();                  ///< @brief 处理瓦片触发事件（从PhysicsEngine获取信息）
    void handlePlayerDamage(int damage);         ///< @brief 处理玩家受伤（更新得分、UI等）
    void playerVSEnemyCollision(engine::object::GameObject* player, engine::object::GameObject* enemy);  ///< @brief 玩家与敌人碰撞处理
    void playerVSItemCollision(engine::object::GameObject* player, engine::object::GameObject* item);    ///< @brief 玩家与道具碰撞处理

    void toNextLevel(engine::object::GameObject* trigger);          ///< @brief 进入下一个关卡
    void showEndScene(bool isWin);                                  ///< @brief 显示结束场景

    /// @brief 根据关卡名称获取对应的地图文件路径
    std::string levelNameToPath(std::string_view levelName) const { return "assets/maps/" + std::string(levelName) + ".tmj"; }

    /**
     * @brief 创建一个特效对象（一次性）。
     * @param centerPos 特效中心位置
     * @param tag 特效标签（决定特效类型,例如"enemy","item"）
     */
    void createEffect(glm::vec2 centerPos, std::string_view tag);

    // --- UI 相关函数 ---
    void createScoreUI();                           ///< @brief 创建得分UI
    void createHealthUI();                          ///< @brief 创建生命值UI (或最大生命值改变时重设)
    void addScoreWithUI(int score);                 ///< @brief 增加得分，同时更新UI
    void healWithUI(int amount);                    ///< @brief 增加生命，同时更新UI
    void updateHealthWithUI();                      ///< @brief 更新生命值UI (只适用最大生命值不变的情况)
};
}