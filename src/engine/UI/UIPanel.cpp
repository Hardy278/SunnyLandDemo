#include "UIPanel.hpp"
#include "../core/Context.hpp"
#include "../render/Renderer.hpp"
#include <SDL3/SDL_pixels.h>
#include <spdlog/spdlog.h>

namespace engine::ui {

UIPanel::UIPanel(glm::vec2 position, glm::vec2 size, std::optional<engine::utils::FColor> backgroundColor)
    : UIElement(std::move(position), std::move(size)), m_backgroundColor(std::move(backgroundColor)) {
    spdlog::trace("UIPanel 构造完成。");
}

void UIPanel::render(engine::core::Context& context) {
    if (!m_visible) return;

    if (m_backgroundColor) {
        context.getRenderer().drawUIFilledRect(getBounds(), m_backgroundColor.value());
    }

    UIElement::render(context); // 调用基类渲染方法(绘制子节点)
}

} // namespace engine::ui 