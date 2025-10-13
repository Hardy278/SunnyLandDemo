#pragma once
#include "UIElement.hpp"
#include "../utils/Math.hpp"
#include <optional>

namespace engine::ui {

/**
 * @brief 用于分组其他UI元素的容器UI元素
 *
 * Panel通常用于布局和组织。
 * 可以选择是否绘制背景色(纯色)。
 */
class UIPanel final : public UIElement {
    std::optional<engine::utils::FColor> m_backgroundColor;    ///< @brief 可选背景色

public:
    /**
     * @brief 构造一个Panel
     *
     * @param position Panel的局部位置  
     * @param size Panel的大小
     * @param backgroundColor 背景色
     */
    explicit UIPanel(glm::vec2 position = {0.0f, 0.0f}, glm::vec2 size = {0.0f, 0.0f}, std::optional<engine::utils::FColor> backgroundColor = std::nullopt);

    void setBackgroundColor(std::optional<engine::utils::FColor> backgroundColor) { m_backgroundColor = std::move(backgroundColor); }
    const std::optional<engine::utils::FColor>& getBackgroundColor() const { return m_backgroundColor; }

    void render(engine::core::Context& context) override;
};

} // namespace engine::ui