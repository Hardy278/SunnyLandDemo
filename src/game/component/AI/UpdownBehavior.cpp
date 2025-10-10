#include "UpdownBehavior.hpp"
#include "../AIComponent.hpp"
#include "../../../engine/component/PhysicsComponent.hpp"
#include "../../../engine/component/TransformComponent.hpp"
#include "../../../engine/component/AnimationComponent.hpp"
#include "../../../engine/object/GameObject.hpp"
#include <spdlog/spdlog.h>

namespace game::component::ai {

UpDownBehavior::UpDownBehavior(float minY, float maxY, float speed)
    : m_patrolMinY(minY), m_patrolMaxY(maxY), m_moveSpeed(speed) {
    if (m_patrolMinY >= m_patrolMaxY) {
        spdlog::error("UpDownBehavior: minY ({}) 应小于 maxY ({})。行为可能不正确。", minY, maxY);
        m_patrolMinY = m_patrolMaxY;  // 修正为相等，避免逻辑错误
    }
}

void UpDownBehavior::enter(AIComponent& AIComponent) {
    // 播放动画 (进行 up-down 行为的对象应该有 'fly' 动画)
    if (auto* animationComponent = AIComponent.getAnimationComponent(); animationComponent) {
        animationComponent->playAnimation("fly");
    }

    // 禁用重力
    if (auto* physicsComponent = AIComponent.getPhysicsComponent(); physicsComponent) {
        physicsComponent->setUseGravity(false);
    }
}

void UpDownBehavior::update(float /*delta_time*/, AIComponent& AIComponent) {
    // 获取必要的组件
    auto* physicsComponent = AIComponent.getPhysicsComponent();
    auto* transform_component = AIComponent.getTransformComponent();
    if (!physicsComponent || !transform_component) {
        spdlog::error("UpdownBehavior：缺少必要的组件，无法执行巡逻行为。");
        return;
    }

    // --- 检查碰撞和边界 ---
    auto currentY = transform_component->getPosition().y;

    // 到达上边界或碰到上方障碍，向下移动
    if (physicsComponent->hasCollidedAbove() || currentY <= m_patrolMinY) {
        physicsComponent->m_velocity.y = m_moveSpeed;
        m_movingDown = true;
    // 到达下边界或碰到下方障碍，向上移动
    } else if (physicsComponent->hasCollidedBelow() || currentY >= m_patrolMaxY) {
        physicsComponent->m_velocity.y = -m_moveSpeed;
        m_movingDown = false;
    }
    /* 不需要翻转精灵图 */
}

} // namespace game::component::ai 