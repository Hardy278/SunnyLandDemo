#include "IdleState.hpp"
#include "FallState.hpp"
#include "JumpState.hpp"
#include "WalkState.hpp"
#include "ClimbState.hpp"
#include "../PlayerComponent.hpp"
#include "../../../engine/core/Context.hpp"
#include "../../../engine/input/InputManager.hpp"
#include "../../../engine/component/PhysicsComponent.hpp"
#include "../../../engine/component/TransformComponent.hpp"
#include <spdlog/spdlog.h>

namespace game::component::state {

void IdleState::enter() {
    playAnimation("idle");
}

void IdleState::exit() {
}

std::unique_ptr<PlayerState> IdleState::update(float, engine::core::Context&) {
    // 应用摩擦力(水平方向)
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    auto friction_factor = m_playerComponent->getFrictionFactor();
    physicsComponent->m_velocity.x *= friction_factor;
    // 如果离地，则切换到 FallState
    if (!m_playerComponent->isOnGround()) {
        return std::make_unique<FallState>(m_playerComponent);
    }
    return nullptr;
}


std::unique_ptr<PlayerState> IdleState::moveLeft() {
    // 切换到 WalkState
    return std::make_unique<WalkState>(m_playerComponent);
}

std::unique_ptr<PlayerState> IdleState::moveRight() {
    // 切换到 WalkState
    return std::make_unique<WalkState>(m_playerComponent);
}

std::unique_ptr<PlayerState> IdleState::jump() {
    // 切换到 JumpState
    return std::make_unique<JumpState>(m_playerComponent);
}

std::unique_ptr<PlayerState> IdleState::climbUp() {
    auto physics_component = m_playerComponent->getPhysicsComponent();
    // 如果与梯子重合，则切换到 ClimbState
    if (physics_component->hasCollidedLadder()) {
        return std::make_unique<ClimbState>(m_playerComponent);
    }

    return nullptr;
}

std::unique_ptr<PlayerState> IdleState::climbDown() {
    auto physics_component = m_playerComponent->getPhysicsComponent();
    // 如果已经在梯子顶层，则切换到 ClimbState
    if (physics_component->isOnTopLadder()) {
        // 需要向下移动一点，确保下一帧能与梯子碰撞（否则会切换回FallState）
        m_playerComponent->getTransformComponent()->translate(glm::vec2(0, 2.0f));
        return std::make_unique<ClimbState>(m_playerComponent);
    }

    return nullptr;
}

} // namespace game::component::state