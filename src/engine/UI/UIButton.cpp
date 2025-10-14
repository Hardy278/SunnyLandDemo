#include "UIButton.hpp"
#include "state/UINormalState.hpp"
#include <spdlog/spdlog.h>

namespace engine::ui {
UIButton::UIButton(
    engine::core::Context& context,
    std::string_view normalSpriteID, 
    std::string_view hoverSpriteID,
    std::string_view pressedSpriteID,
    glm::vec2 position,
    glm::vec2 size, 
    std::function<void()> callback
) : UIInteractive(context, std::move(position), std::move(size)), m_callback(std::move(callback)) {
    addSprite("normal", std::make_unique<engine::render::Sprite>(normalSpriteID));
    addSprite("hover", std::make_unique<engine::render::Sprite>(hoverSpriteID));
    addSprite("pressed", std::make_unique<engine::render::Sprite>(pressedSpriteID));

    // 设置默认状态为"normal"
    setState(std::make_unique<engine::ui::state::UINormalState>(this));
    spdlog::trace("UIBUTTOM::UIButton 构造完成");
}

void UIButton::clicked() {
    if (m_callback) m_callback();
}

} // namespace engine::ui