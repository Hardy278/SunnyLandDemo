#include "UIInteractive.hpp"
#include "state/UIState.hpp"
#include "../core/Context.hpp"
#include "../render/Renderer.hpp"
#include "../resource/ResourceManager.hpp"
#include <spdlog/spdlog.h>

namespace engine::ui {

UIInteractive::~UIInteractive() = default;

UIInteractive::UIInteractive(engine::core::Context &context, glm::vec2 position, glm::vec2 size)
    : UIElement(std::move(position), std::move(size)), m_context(context) {
    spdlog::trace("UIINTERACTIVE::UIInteractive 构造完成");
}

void UIInteractive::setState(std::unique_ptr<engine::ui::state::UIState> state) {
    if (!state) {
        spdlog::warn("UIINTERACTIVE::setState::尝试设置空的状态！");
        return;
    }
    m_state = std::move(state);
    m_state->enter();
}

void UIInteractive::addSprite(std::string_view name, std::unique_ptr<engine::render::Sprite> sprite) {
    // 可交互UI元素必须有一个size用于交互检测，因此如果参数列表中没有指定，则用图片大小作为size
    if (m_size.x == 0.0f && m_size.y == 0.0f) {
        m_size = m_context.getResourceManager().getTextureSize(sprite->getTextureID());
    }
    // 添加精灵
    m_sprites[std::string(name)] = std::move(sprite);
}

void UIInteractive::setSprite(std::string_view name) {
    if (m_sprites.find(std::string(name)) != m_sprites.end()) {
        m_currentSprite = m_sprites[std::string(name)].get();
    } else {
        spdlog::warn("Sprite '{}' 未找到", name);
    }
}

bool UIInteractive::handleInput(engine::core::Context &context) {
    if (UIElement::handleInput(context)) {  
        return true;
    }
    // 先更新子节点，再更新自己（状态）
    if (m_state && m_interactive) {
        if (auto nextState = m_state->handleInput(context); nextState) {
            setState(std::move(nextState));
            return true;
        }
    }
    return false;
}

void UIInteractive::render(engine::core::Context &context) {
    if (!m_visible) return;

    // 先渲染自身
    context.getRenderer().drawUISprite(*m_currentSprite, getScreenPosition(), m_size);

    // 再渲染子元素（调用基类方法）
    UIElement::render(context);
}

} // namespace engine::ui