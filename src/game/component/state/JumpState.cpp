#include "JumpState.hpp"
#include "IdleState.hpp"
#include "WalkState.hpp"
#include "FallState.hpp"
#include "ClimbState.hpp"
#include "../PlayerComponent.hpp"
#include "../../../engine/core/Context.hpp"
#include "../../../engine/input/InputManager.hpp"
#include "../../../engine/component/PhysicsComponent.hpp"
#include "../../../engine/component/SpriteComponent.hpp"
#include <glm/common.hpp>
#include <spdlog/spdlog.h>

namespace game::component::state {

void JumpState::enter() {
    playAnimation("jump");
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    physicsComponent->m_velocity.y = -m_playerComponent->getJumpVelocity();     // 向上跳跃
    spdlog::debug("JUMPSTATE::enter::DEBUG::PlayerComponent 进入 JumpState, 设置初始垂直速度为: {}", physicsComponent->m_velocity.y);
}

void JumpState::exit() {
}

std::unique_ptr<PlayerState> JumpState::handleInput(engine::core::Context& context) {
    auto inputManager = context.getInputManager();
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    auto spriteComponent = m_playerComponent->getSpriteComponent();
    // 如果按下上下键，且与梯子重合，则切换到 ClimbState
    if (physicsComponent->hasCollidedLadder() &&
        (inputManager.isActionDown("move_up") || inputManager.isActionDown("move_down"))) {
        return std::make_unique<ClimbState>(m_playerComponent);
    }
    // 跳跃状态下可以左右移动
    if (inputManager.isActionDown("move_left")) {
        if (physicsComponent->m_velocity.x > 0.0f) physicsComponent->m_velocity.x = 0.0f;
        physicsComponent->addForce({-m_playerComponent->getMoveForce(), 0.0f});
        spriteComponent->setFlipped(true);
    } else if (inputManager.isActionDown("move_right")) {
        if (physicsComponent->m_velocity.x < 0.0f) physicsComponent->m_velocity.x = 0.0f;
        physicsComponent->addForce({m_playerComponent->getMoveForce(), 0.0f});
        spriteComponent->setFlipped(false);
    }
    return nullptr;
}

std::unique_ptr<PlayerState> JumpState::update(float, engine::core::Context&) {
    // 限制最大速度(水平方向)
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    auto maxSpeed = m_playerComponent->getMaxSpeed();
    physicsComponent->m_velocity.x = glm::clamp(physicsComponent->m_velocity.x, -maxSpeed, maxSpeed);
    // 如果速度为正，切换到 FallState
    if (physicsComponent->m_velocity.y >= 0.0f) {
        return std::make_unique<FallState>(m_playerComponent);
    }
    return nullptr;
}

} // namespace game::component::state