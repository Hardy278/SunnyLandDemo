#include "JumpBehavior.hpp"
#include "../AIComponent.hpp"
#include "../../../engine/component/PhysicsComponent.hpp"
#include "../../../engine/component/TransformComponent.hpp"
#include "../../../engine/component/SpriteComponent.hpp"
#include "../../../engine/component/AnimationComponent.hpp"
#include "../../../engine/object/GameObject.hpp"
#include <spdlog/spdlog.h>

namespace game::component::ai {

JumpBehavior::JumpBehavior(float minX, float maxX, glm::vec2 jumpVel, float jumpInterval)
    : m_patrolMinX(minX), m_patrolMaxX(maxX), m_jumpVel(jumpVel), m_jumpInterval(jumpInterval) {
    if (m_patrolMinX >= m_patrolMaxX) {   // 确保巡逻范围是有效的
        spdlog::error("JumpBehavior: minX ({}) 应小于 maxX ({})。行为可能不正确。", minX, maxX);
        m_patrolMinX = m_patrolMaxX;
    }
    if (m_jumpInterval <= 0.0f) {   // 确保跳跃间隔是正数
        spdlog::error("JumpBehavior: jumpInterval ({}) 应为正数。已设置为 2.0f。", jumpInterval);
        m_jumpInterval = 2.0f;
    }
    if (m_jumpVel.y > 0) {        // 确保垂直跳跃速度是负数（向上）
        spdlog::error("JumpBehavior: 垂直跳跃速度 ({}) 应为负数（向上）。已取相反数。", m_jumpVel.y);
        m_jumpVel.y = -m_jumpVel.y;
    }
}

void JumpBehavior::update(float deltaTime, AIComponent& AIComponent) {
    // 获取必要的组件
    auto* physicsComponent = AIComponent.getPhysicsComponent();
    auto* transformComponent = AIComponent.getTransformComponent();
    auto* spriteComponent = AIComponent.getSpriteComponent();
    auto* animationComponent = AIComponent.getAnimationComponent();
    if (!physicsComponent || !transformComponent || !spriteComponent || !animationComponent) {
        spdlog::error("JumpBehavior: 缺少必要的组件，无法执行跳跃行为。");
        return;
    }

    auto isOnGround = physicsComponent->hasCollidedBelow();      // 着地标志
    if (isOnGround) {    // 如果在地面上
        m_jumpTimer += deltaTime;              // 增加跳跃计时器
        physicsComponent->m_velocity.x = 0.0f;  // 停止水平移动（否则会有惯性）
        if (m_jumpTimer >= m_jumpInterval) {    // 时间到，准备跳跃
            m_jumpTimer = 0.0f; // 重置计时器
            // --- 检查是否需要更新跳跃方向 ---
            auto currentX = transformComponent->getPosition().x;
            // 如果右边超限或者撞墙，向左跳
            if (m_jumpingRight && (physicsComponent->hasCollidedRight() || currentX >= m_patrolMaxX)) {
                m_jumpingRight = false;
            // 如果左边超限或者撞墙，向右跳
            } else if (!m_jumpingRight && (physicsComponent->hasCollidedLeft() || currentX <= m_patrolMinX)) {
                m_jumpingRight = true;
            }
            auto m_jumpVelx = m_jumpingRight ? m_jumpVel.x : -m_jumpVel.x;  // 确定水平跳跃方向
            physicsComponent->m_velocity = {m_jumpVelx, m_jumpVel.y};        // 设置速度
            animationComponent->playAnimation("jump");     // 播放跳跃动画
            spriteComponent->setFlipped(m_jumpingRight);   // 更新精灵翻转
        } else {    // 还在地面等待
             animationComponent->playAnimation("idle");
        }
    } else {    // 在空中, 根据垂直速度判断是上升(jump)还是下落(fall)
        if (physicsComponent->getVelocity().y < 0) {
            animationComponent->playAnimation("jump");
        } else {
            animationComponent->playAnimation("fall");
        }
    }
}

} // namespace game::component::ai 