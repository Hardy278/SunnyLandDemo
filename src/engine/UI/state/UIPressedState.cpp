#include "UIHoverState.hpp"
#include "UINormalState.hpp"
#include "UIPressedState.hpp"
#include "../UIInteractive.hpp"
#include "../../input/InputManager.hpp"
#include "../../core/Context.hpp"
#include <spdlog/spdlog.h>

namespace engine::ui::state {

void UIPressedState::enter() {
    m_owner->setSprite("pressed");
    spdlog::debug("切换到按下状态");
}

std::unique_ptr<UIState> UIPressedState::handleInput(engine::core::Context& context) {
    auto& inputManager = context.getInputManager();
    auto mouse_pos = inputManager.getLogicalMousePosition();
    if (inputManager.isActionReleased("MouseLeftClick")) {
        if (!m_owner->isPointInside(mouse_pos)) {        // 松开鼠标时，如果不在UI元素内，则切换到正常状态
            return std::make_unique<engine::ui::state::UINormalState>(m_owner);
        } else {                                        // 松开鼠标时，如果还在UI元素内，则触发点击事件
            m_owner->clicked();
            return std::make_unique<engine::ui::state::UIHoverState>(m_owner);
        }
    }
    return nullptr;
}

} // namespace engine::ui::state