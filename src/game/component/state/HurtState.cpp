#include "HurtState.hpp"
#include "IdleState.hpp"
#include "WalkState.hpp"
#include "FallState.hpp"
#include "../PlayerComponent.hpp"
#include "../../../engine/core/Context.hpp"
#include "../../../engine/component/PhysicsComponent.hpp"
#include "../../../engine/component/SpriteComponent.hpp"
#include <glm/common.hpp>

namespace game::component::state {

void HurtState::enter() {
    playAnimation("hurt");  // 播放受伤动画
    // --- 造成击退效果 ---
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    auto spriteComponent = m_playerComponent->getSpriteComponent();
    auto knockbackVelocity = glm::vec2(-100.0f, -150.0f);      // 默认左上方击退效果
    // 根据当前精灵的朝向状态决定是否改成右上方
    if (spriteComponent->isFlipped()) {
        knockbackVelocity.x = -knockbackVelocity.x;  // 变成向右
    }
    physicsComponent->m_velocity = knockbackVelocity;  // 设置击退速度
}

void HurtState::exit() {
}

std::unique_ptr<PlayerState> HurtState::handleInput(engine::core::Context&){
    // 硬直期不能进行任何操控
    return nullptr;
}

std::unique_ptr<PlayerState> HurtState::update(float delta_time, engine::core::Context&){
    m_stunnedTimer += delta_time;
    // --- 两种情况离开受伤（硬直）状态：---
    // 1. 落地
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    if (physicsComponent->hasCollidedBelow()) {
        if (glm::abs(physicsComponent->m_velocity.x) < 1.0f) {
            return std::make_unique<IdleState>(m_playerComponent);
        } else {
            return std::make_unique<WalkState>(m_playerComponent);
        }
    }
    // 2. 硬直时间结束(能走到这里说明没有落地，直接切换到 FallState)
    if (m_stunnedTimer > m_playerComponent->getStunnedDuration()){
        m_stunnedTimer = 0.0f;  // 重置硬直计时器
        return std::make_unique<FallState>(m_playerComponent);  // 切换到下落状态
    }
    return nullptr;
}

}