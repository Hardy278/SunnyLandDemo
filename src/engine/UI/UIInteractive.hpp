#pragma once
#include "UIElement.hpp"
#include "state/UIState.hpp"
#include "../render/Sprite.hpp"   // 需要引入头文件而不是前置声明（map容器创建时可能会检查内部元素是否有析构定义）
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace engine::core {
    class Context;
}

namespace engine::ui {

/**
 * @brief 可交互UI元素的基类,继承自UIElement
 *
 * 定义了可交互UI元素的通用属性和行为。
 * 管理UI状态的切换和交互逻辑。
 * 提供事件处理、更新和渲染的虚方法。
 */
class UIInteractive : public UIElement {
protected:
    engine::core::Context& m_context;                        ///< @brief 可交互元素很可能需要其他引擎组件
    std::unique_ptr<engine::ui::state::UIState> m_state;     ///< @brief 当前状态
    std::unordered_map<std::string, std::unique_ptr<engine::render::Sprite>> m_sprites; ///< @brief 精灵集合
    engine::render::Sprite* m_currentSprite = nullptr;       ///< @brief 当前显示的精灵
    bool m_interactive = true;                               ///< @brief 是否可交互

public:
    UIInteractive(engine::core::Context& context, glm::vec2 position = {0.0f, 0.0f}, glm::vec2 size = {0.0f, 0.0f});
    ~UIInteractive() override;

    virtual void clicked() {}       ///< @brief 如果有点击事件，则重写该方法

    void addSprite(std::string_view name, std::unique_ptr<engine::render::Sprite> sprite);///< @brief 添加精灵
    void setSprite(std::string_view name);                                                ///< @brief 设置当前显示的精灵
    // --- Getters and Setters ---
    void setState(std::unique_ptr<engine::ui::state::UIState> state);       ///< @brief 设置当前状态
    engine::ui::state::UIState* getState() const { return m_state.get(); }   ///< @brief 获取当前状态

    void setInteractive(bool interactive) { m_interactive = interactive; }   ///< @brief 设置是否可交互
    bool isInteractive() const { return m_interactive; }                     ///< @brief 获取是否可交互

    // --- 核心方法 ---
    bool handleInput(engine::core::Context& context) override;
    void render(engine::core::Context& context) override;
};

} // namespace engine::ui