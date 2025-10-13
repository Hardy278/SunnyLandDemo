#pragma once
#include "../../engine/scene/Scene.hpp"
#include <memory>

// 前向声明
namespace engine::core {
    class Context;
}
namespace engine::scene {
    class SceneManager;
}
namespace game::data {
    class SessionData;
}

namespace game::scene {

/**
 * @brief 游戏暂停时显示的菜单场景，提供继续、保存、返回、退出等选项。
 * 该场景通常被推送到 GameScene 之上。
 */
class MenuScene final : public engine::scene::Scene {
private:
    std::shared_ptr<game::data::SessionData> m_sessionData;

public:
    /**
     * @brief MenuScene 的构造函数
     * @param context 引擎上下文的引用
     * @param sceneManager 场景管理器的引用
     * @param sessionData 场景间传递的游戏数据
     */
    MenuScene(
        engine::core::Context& context,
        engine::scene::SceneManager& sceneManager,
        std::shared_ptr<game::data::SessionData> sessionData
    );

    ~MenuScene() override = default;

    // --- 核心循环方法 ---
    void init() override;
    void handleInput() override;

private:
    // 用于创建UI和按钮回调的私有辅助方法
    void createUI();
    void onResumeClicked();
    void onSaveClicked();
    void onBackClicked();
    void onQuitClicked();

};

} // namespace game::scene