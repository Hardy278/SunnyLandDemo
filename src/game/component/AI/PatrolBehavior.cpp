#include "PatrolBehavior.hpp"
#include "../AIComponent.hpp"
#include "../../../engine/component/PhysicsComponent.hpp"
#include "../../../engine/component/TransformComponent.hpp"
#include "../../../engine/component/SpriteComponent.hpp"
#include "../../../engine/component/AnimationComponent.hpp"
#include "../../../engine/object/GameObject.hpp"
#include <spdlog/spdlog.h>

namespace game::component::ai {

PatrolBehavior::PatrolBehavior(float minX, float maxX, float speed)
    : m_patrolMinX(minX), m_patrolMaxX(maxX), m_moveSpeed(speed) {
    if (m_patrolMinX >= m_patrolMaxX) {
        spdlog::error("PatrolBehavior: minX ({}) 应小于 maxX ({})。行为可能不正确。", minX, maxX);
        m_patrolMinX = m_patrolMaxX;  // 修正为相等，避免逻辑错误
    }
}

void PatrolBehavior::enter(AIComponent& AIComponent) {
    // 播放动画 (进行 patrol 行为的对象应该有 'walk' 动画)
    if (auto* animationComponent = AIComponent.getAnimationComponent(); animationComponent) {
        animationComponent->playAnimation("walk");
    }
}

void PatrolBehavior::update(float /*delta_time*/, AIComponent& AIComponent) {
    // 获取必要的组件
    auto* physicsComponent = AIComponent.getPhysicsComponent();
    auto* transformComponent = AIComponent.getTransformComponent();
    auto* spriteComponent = AIComponent.getSpriteComponent();
    if (!physicsComponent || !transformComponent || !spriteComponent) {
        spdlog::error("PatrolBehavior: 缺少必要的组件，无法执行巡逻行为。");
        return;
    }

    // --- 检查碰撞和边界 ---
    auto currentX = transformComponent->getPosition().x;

    // 撞右墙或到达设定目标则转向左
    if (physicsComponent->hasCollidedRight() || currentX >= m_patrolMaxX) {
        physicsComponent->m_velocity.x = -m_moveSpeed;
        m_movingRight = false;
    // 撞墙左或到达设定目标则转向右
    }else if (physicsComponent->hasCollidedLeft() || currentX <= m_patrolMinX) {
        physicsComponent->m_velocity.x = m_moveSpeed;
        m_movingRight = true;
    }

    // 更新精灵翻转(向左移动时，不翻转)
    spriteComponent->setFlipped(m_movingRight);
}

} // namespace game::component::ai 