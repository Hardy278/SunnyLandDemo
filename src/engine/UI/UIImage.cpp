#include "UIImage.hpp"
#include "../render/Renderer.hpp"
#include "../render/Sprite.hpp"
#include "../core/Context.hpp"
#include <spdlog/spdlog.h>

namespace engine::ui {

UIImage::UIImage(
    std::string_view textureID,
    glm::vec2 position,
    glm::vec2 size,
    std::optional<SDL_FRect> sourceRect,
    bool isFlipped
) : UIElement(std::move(position), std::move(size)), m_sprite(textureID, std::move(sourceRect), isFlipped) {
    if (textureID.empty()) {
        spdlog::warn("UIIMAGE::创建了一个空纹理ID的UIImage。");
    }
    spdlog::trace("UIIMAGE::UIImage 构造完成");
}

void UIImage::render(engine::core::Context& context) {
    if (!m_visible || m_sprite.getTextureID().empty()) {
        return; // 如果不可见或没有分配纹理则不渲染
    }

    // 渲染自身
    auto position = getScreenPosition();
    if (m_size.x == 0.0f && m_size.y == 0.0f) {   // 如果尺寸为0，则使用纹理的原始尺寸
        context.getRenderer().drawUISprite(m_sprite, position);
    } else {
        context.getRenderer().drawUISprite(m_sprite, position, m_size);
    }

    // 渲染子元素（调用基类方法）
    UIElement::render(context);
}

} // namespace engine::ui 