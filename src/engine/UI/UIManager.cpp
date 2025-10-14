#include "UIManager.hpp"
#include "UIPanel.hpp"
#include "UIElement.hpp"
#include <spdlog/spdlog.h>

namespace engine::ui {

UIManager::~UIManager() = default;

UIManager::UIManager() {
    // 创建一个无特定大小和位置的Panel，它的子元素将基于它定位。
    m_rootElement = std::make_unique<UIPanel>(glm::vec2{0.0f, 0.0f}, glm::vec2{0.0f, 0.0f});
    spdlog::trace("UIMANAGER::UI管理器构造完成。");
}

bool UIManager::init(const glm::vec2& windowSize) {
    m_rootElement->setSize(windowSize);
    spdlog::trace("UIMANAGER::init::UI管理器已初始化根面板。");
    return true;
}   

void UIManager::addElement(std::unique_ptr<UIElement> element) {
    if (m_rootElement) {
        m_rootElement->addChild(std::move(element));
    } else {
        spdlog::error("UIMANAGER::addElement::无法添加元素: m_rootElement 为空！");
    }
}

void UIManager::clearElements() {
    if (m_rootElement) {
        m_rootElement->removeAllChildren();
        spdlog::trace("UIMANAGER::clearElements::所有UI元素已从UI管理器中清除。");
    }
}

bool UIManager::handleInput(engine::core::Context& context) {
    if (m_rootElement && m_rootElement->isVisible()) {
        // 从根元素开始向下分发事件
        if (m_rootElement->handleInput(context)) return true;
    }
    return false;
}

void UIManager::update(float deltaTime, engine::core::Context& context) {
    if (m_rootElement && m_rootElement->isVisible()) {
        // 从根元素开始向下更新
        m_rootElement->update(deltaTime, context);
    }
}

void UIManager::render(engine::core::Context& context) {
    if (m_rootElement && m_rootElement->isVisible()) {
        // 从根元素开始向下渲染
        m_rootElement->render(context);
    }
}

UIPanel* UIManager::getRootElement() const {
    return m_rootElement.get();
}

} // namespace engine::ui 