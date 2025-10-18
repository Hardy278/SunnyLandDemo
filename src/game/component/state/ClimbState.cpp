#include "ClimbState.hpp"
#include "JumpState.hpp"
#include "IdleState.hpp"
#include "WalkState.hpp"
#include "FallState.hpp"
#include "../PlayerComponent.hpp"
#include "../../../engine/core/Context.hpp"
#include "../../../engine/input/InputManager.hpp"
#include "../../../engine/component/PhysicsComponent.hpp"
#include "../../../engine/component/SpriteComponent.hpp"
#include "../../../engine/component/AnimationComponent.hpp"

#include <glm/common.hpp>
#include <spdlog/spdlog.h>

namespace game::component::state {

void ClimbState::enter() {
    spdlog::debug("CLIMBSTATE::enter::DEBUG::进入攀爬状态");
    playAnimation("climb");
    if (auto* physics = m_playerComponent->getPhysicsComponent(); physics) {
        physics->setUseGravity(false); // 禁用重力
    }
}

void ClimbState::exit() {
    spdlog::debug("CLIMBSTATE::exit::DEBUG::退出攀爬状态");
    if (auto* physics = m_playerComponent->getPhysicsComponent(); physics) {
        physics->setUseGravity(true); // 重新启用重力
    }
}


std::unique_ptr<PlayerState> ClimbState::update(float, engine::core::Context&) {
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    auto animationComponent = m_playerComponent->getAnimationComponent();

    // 根据是否正在移动，决定是否播放动画
    m_isMoving ? animationComponent->resumeAnimation() : animationComponent->stopAnimation();

    // 如果着地，则切换到 IdleState
    if (physicsComponent->hasCollidedBelow()) {
        return std::make_unique<IdleState>(m_playerComponent);
    }
    // 如果离开梯子区域，则切换到 FallState（能走到这里 说明非着地状态）
    if (!physicsComponent->hasCollidedLadder()) {
        return std::make_unique<FallState>(m_playerComponent);
    }

    m_isMoving = false;         // 循环的最后重置移动标志
    physicsComponent->m_velocity = glm::vec2(0.0f, 0.0f);  // 速度归零(没有操控就静止不动)
    return nullptr;
}

std::unique_ptr<PlayerState> ClimbState::moveLeft() {
    auto physicsComponent = m_playerComponent->getPhysicsComponent();

    // 水平向左移动
    physicsComponent->m_velocity.x = -m_playerComponent->getClimbSpeed();
    m_isMoving = true;
    return nullptr;
}

std::unique_ptr<PlayerState> ClimbState::moveRight() {
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    // 水平向右移动
    physicsComponent->m_velocity.x = m_playerComponent->getClimbSpeed();
    m_isMoving = true;
    return nullptr;
}

std::unique_ptr<PlayerState> ClimbState::jump() {
    // 直接切换到 JumpState
    return std::make_unique<JumpState>(m_playerComponent);
}

std::unique_ptr<PlayerState> ClimbState::climbUp() {
    auto physicsComponent = m_playerComponent->getPhysicsComponent();

    // 向上移动
    physicsComponent->m_velocity.y = -m_playerComponent->getClimbSpeed();
    m_isMoving = true;
    return nullptr;
}

std::unique_ptr<PlayerState> ClimbState::climbDown() {
    auto physicsComponent = m_playerComponent->getPhysicsComponent();

    // 向下移动
    physicsComponent->m_velocity.y = m_playerComponent->getClimbSpeed();
    m_isMoving = true;
    return nullptr;
}

} // namespace game::component::state