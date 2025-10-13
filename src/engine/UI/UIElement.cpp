#include "UIElement.hpp"
#include "../core/Context.hpp"
#include <algorithm>
#include <utility>
#include <spdlog/spdlog.h>

namespace engine::ui {

UIElement::UIElement(glm::vec2 position, glm::vec2 size)
    : m_position(std::move(position)), m_size(std::move(size)) {
}   

bool UIElement::handleInput(engine::core::Context& context) {
    // 如果元素不可见，直接返回 false
    if (!m_visible) return false; 

    // 遍历所有子节点，并删除标记了移除的元素
    for (auto it = m_children.begin(); it != m_children.end();) {
        if (*it && !(*it)->isNeedRemove()) {
            if ((*it)->handleInput(context)) return true;
            ++it;
        } else {
            it = m_children.erase(it);
        }
    }
    // 事件未被消耗，返回假
    return false;
}

void UIElement::update(float deltaTime, engine::core::Context& context) {
    if (!m_visible) return;

    // 遍历所有子节点，并删除标记了移除的元素
    for (auto it = m_children.begin(); it != m_children.end();) {
        if (*it && !(*it)->isNeedRemove()) {
            (*it)->update(deltaTime, context);
            ++it;
        } else {
            it = m_children.erase(it);
        }
    }
}

void UIElement::render(engine::core::Context& context) {
    if (!m_visible) return;

    // 渲染子元素
    for (const auto& child : m_children) {
        if (child) child->render(context);
    }
}

void UIElement::addChild(std::unique_ptr<UIElement> child) {
    if (child) {
        child->setParent(this); // 设置父指针
        m_children.push_back(std::move(child));
    }
}

std::unique_ptr<UIElement> UIElement::removeChild(UIElement* childPtr) {
    // 使用 std::remove_if 和 lambda 表达式自定义比较的方式移除
    auto it = std::find_if(m_children.begin(), m_children.end(), [childPtr](const std::unique_ptr<UIElement>& p) {
        return p.get() == childPtr; 
    });

    if (it != m_children.end()) {
        std::unique_ptr<UIElement> removedChild = std::move(*it);
        m_children.erase(it);
        removedChild->setParent(nullptr);      // 清除父指针
        return removedChild;                   // 返回被移除的子元素（可以挂载到别处）
    }
    return nullptr; // 未找到子元素
}

void UIElement::removeAllChildren() {
    for (auto& child : m_children) {
        child->setParent(nullptr); // 清除父指针
    }
    m_children.clear();
}

glm::vec2 UIElement::getScreenPosition() const {
    if (m_parent) {
        return m_parent->getScreenPosition() + m_position;
    }
    return m_position; // 根元素的位置已经是相对屏幕的绝对位置
}

engine::utils::Rect UIElement::getBounds() const {
    auto absPos = getScreenPosition();
    return engine::utils::Rect(absPos, m_size);
}

bool UIElement::isPointInside(const glm::vec2& point) const {
    auto bounds = getBounds();
    return (point.x >= bounds.position.x && point.x < (bounds.position.x + bounds.size.x) &&
            point.y >= bounds.position.y && point.y < (bounds.position.y + bounds.size.y));
}

} // namespace engine::ui 