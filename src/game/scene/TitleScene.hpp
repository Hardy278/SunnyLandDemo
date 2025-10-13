#pragma once
#include "../../engine/scene/Scene.hpp"
#include <memory>

namespace engine::object {
    class GameObject;
}

namespace engine::scene {
    class SceneManager;
}

namespace game::data {
    class SessionData;
}

namespace game::scene {

/**
 * @brief 标题场景类，提供4个按钮：开始游戏、加载游戏、帮助、退出
 */
class TitleScene final : public engine::scene::Scene {
private:
    std::shared_ptr<game::data::SessionData> m_sessionData;

public:
    /**
     * @brief 构造函数
     * @param context 引擎上下文
     * @param sceneManager 场景管理器
     * @param sessionData 会话数据，默认为空
     */
    TitleScene(
        engine::core::Context& context,
        engine::scene::SceneManager& sceneManager,
        std::shared_ptr<game::data::SessionData> sessionData = nullptr
    );

    ~TitleScene() override = default;

    // --- 核心方法 --- //
    void init() override;
    void update(float delta_time) override;

    // 禁止拷贝和移动
    TitleScene(const TitleScene&) = delete;
    TitleScene& operator=(const TitleScene&) = delete;
    TitleScene(TitleScene&&) = delete;
    TitleScene& operator=(TitleScene&&) = delete;

private:
    // 初始化 UI 元素
    void createUI();

    // 按钮回调函数
    void onStartGameClick();
    void onLoadGameClick();
    void onHelpsClick();
    void onQuitClick();
};

} // namespace game::scenes