#include "UIHoverState.hpp"
#include "UINormalState.hpp"
#include "../UIInteractive.hpp"
#include "../../input/InputManager.hpp"
#include "../../core/Context.hpp"
#include <spdlog/spdlog.h>

namespace engine::ui::state {

void UINormalState::enter() {
    m_owner->setSprite("normal");
    spdlog::debug("UINORMALSTATE::切换到正常状态");
}

std::unique_ptr<UIState> UINormalState::handleInput(engine::core::Context& context) {
    auto& inputManager = context.getInputManager();
    auto mousePos = inputManager.getLogicalMousePosition();
    if (m_owner->isPointInside(mousePos)) {         // 如果鼠标在UI元素内，则切换到悬停状态
        return std::make_unique<engine::ui::state::UIHoverState>(m_owner);
    }
    return nullptr;
}

} // namespace engine::ui::state