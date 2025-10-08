#include "ColliderComponent.hpp"
#include "TransformComponent.hpp"
#include "../object/GameObject.hpp"
#include "../physics/Collider.hpp"
#include <spdlog/spdlog.h>

namespace engine::component {

ColliderComponent::ColliderComponent(
    std::unique_ptr<engine::physics::Collider> collider,
    engine::utils::Alignment alignment,
    bool isTrigger, bool isActive
) : m_collider(std::move(collider)), m_alignment(alignment), m_isTrigger(isTrigger), m_isActive(isActive) {
    if (!m_collider) {
        spdlog::error("COLLIDERCOMPONENT::ERROR::创建 ColliderComponent 时传入了空的碰撞器！");
    }
}

void ColliderComponent::init() {
    if (!m_owner) {
        spdlog::error("COLLIDERCOMPONENT::init::ERROR::ColliderComponent 没有所有者 GameObject！");
        return;
    }
    m_transform = m_owner->getComponent<TransformComponent>();
    if (!m_transform) {
        spdlog::error("COLLIDERCOMPONENT::init::ERROR::ColliderComponent 需要一个在同一个 GameObject 上的 TransformComponent！");
        return;
    }
    // 在获取 m_transform 之后计算初始偏移量
    updateOffset();
}

// 实现 setAlignment 方法
void ColliderComponent::setAlignment(engine::utils::Alignment anchor) {
    m_alignment = anchor;
    // 重新计算偏移量，确保 m_transform 和 m_collider 有效
    if (m_transform && m_collider) {
        updateOffset();
    }
}

void ColliderComponent::updateOffset() {
    if (!m_collider) return;
    // 获取碰撞盒的最小包围盒尺寸
    auto colliderSize = m_collider->getAABBSize();
    // 如果尺寸无效，偏移为0
    if (colliderSize.x <= 0.0f || colliderSize.y <= 0.0f) {
        m_offset = {0.0f, 0.0f};
        return;
    }
    auto scale = m_transform->getScale(); // 获取 Transform 的缩放
    // 根据 m_alignment 计算 AABB 左上角相对于 Transform 中心的偏移量
    switch (m_alignment) {
        case engine::utils::Alignment::TOP_LEFT:
            m_offset = glm::vec2{0.0f, 0.0f} * scale;
            break;
        case engine::utils::Alignment::TOP_CENTER:
            m_offset = glm::vec2{-colliderSize.x / 2.0f, 0.0f} * scale;
            break;
        case engine::utils::Alignment::TOP_RIGHT:
            m_offset = glm::vec2{-colliderSize.x, 0.0f} * scale;
            break;
        case engine::utils::Alignment::CENTER_LEFT:
            m_offset = glm::vec2{0.0f, -colliderSize.y / 2.0f} * scale;
            break;
        case engine::utils::Alignment::CENTER:
            m_offset = glm::vec2{-colliderSize.x / 2.0f, -colliderSize.y / 2.0f} * scale;
            break;
        case engine::utils::Alignment::CENTER_RIGHT:
             m_offset = glm::vec2{-colliderSize.x, -colliderSize.y / 2.0f} * scale;
            break;
        case engine::utils::Alignment::BOTTOM_LEFT:
            m_offset = glm::vec2{0.0f, -colliderSize.y} * scale;
            break;
        case engine::utils::Alignment::BOTTOM_CENTER:
            m_offset = glm::vec2{-colliderSize.x / 2.0f, -colliderSize.y} * scale;
            break;
        case engine::utils::Alignment::BOTTOM_RIGHT:
            m_offset = glm::vec2{-colliderSize.x, -colliderSize.y} * scale;
            break;
        default:
            break; // 如果 m_alignment 是 NONE，则不做任何操作（手动设置 m_offset）
    }
}

engine::utils::Rect ColliderComponent::getWorldAABB() const {
    if (!m_transform || !m_collider) {
        return {glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
    }
    // 计算最小包围盒的左上角坐标（position）
    const glm::vec2 topLeftPos = m_transform->getPosition() + m_offset;
    // 计算最小包围盒的尺寸（size）
    const glm::vec2 baseSize = m_collider->getAABBSize();
    const glm::vec2 scale = m_transform->getScale();
    glm::vec2 scaledSize = baseSize * scale;
    // 返回最小包围盒的 Rect
    return { topLeftPos, scaledSize };
}

} // namespace engine::component 