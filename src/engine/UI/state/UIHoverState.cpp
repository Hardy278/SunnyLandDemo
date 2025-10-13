#include "UIHoverState.hpp"
#include "UINormalState.hpp"
#include "UIPressedState.hpp"
#include "../UIInteractive.hpp"
#include "../../input/InputManager.hpp"
#include "../../core/Context.hpp"
#include <spdlog/spdlog.h>

namespace engine::ui::state {

void UIHoverState::enter() {
    m_owner->setSprite("hover");
    spdlog::debug("切换到悬停状态");
}

std::unique_ptr<UIState> UIHoverState::handleInput(engine::core::Context& context) {
    auto& inputManager = context.getInputManager();
    auto mousePos = inputManager.getLogicalMousePosition();
    if (!m_owner->isPointInside(mousePos)) {                // 如果鼠标不在UI元素内，则返回正常状态
        return std::make_unique<UINormalState>(m_owner);
    }
    if (inputManager.isActionPressed("MouseLeftClick")) {  // 如果鼠标按下，则返回按下状态
        return std::make_unique<UIPressedState>(m_owner);
    }
    return nullptr;
}

} // namespace engine::ui::state