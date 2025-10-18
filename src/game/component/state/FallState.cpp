#include "FallState.hpp"
#include "IdleState.hpp"
#include "WalkState.hpp"
#include "ClimbState.hpp"
#include "../PlayerComponent.hpp"
#include "../../../engine/core/Context.hpp"
#include "../../../engine/input/InputManager.hpp"
#include "../../../engine/component/PhysicsComponent.hpp"
#include "../../../engine/component/SpriteComponent.hpp"
#include <glm/common.hpp>

namespace game::component::state {

void FallState::enter() {
    playAnimation("fall");
}

void FallState::exit() {
}


std::unique_ptr<PlayerState> FallState::update(float, engine::core::Context&) {
    // 限制最大速度(水平方向)
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    auto maxSpeed = m_playerComponent->getMaxSpeed();
    physicsComponent->m_velocity.x = glm::clamp(physicsComponent->m_velocity.x, -maxSpeed, maxSpeed);

    // 如果下方有碰撞，则根据水平速度来决定 切换到 IdleState 或 WalkState
    if (physicsComponent->hasCollidedBelow()) {
        if (glm::abs(physicsComponent->m_velocity.x) < 1.0f) {
            return std::make_unique<IdleState>(m_playerComponent);
        } else {
            return std::make_unique<WalkState>(m_playerComponent);
        }
    }
    return nullptr;
}



std::unique_ptr<PlayerState> FallState::moveLeft() {
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    auto spriteComponent = m_playerComponent->getSpriteComponent();
    // 如果当前速度是向右的，则先减速到0 (增强操控手感)
    if (physicsComponent->m_velocity.x > 0.0f) physicsComponent->m_velocity.x = 0.0f;
    physicsComponent->addForce({-m_playerComponent->getMoveForce(), 0.0f});
    spriteComponent->setFlipped(true);

    return nullptr;
}

std::unique_ptr<PlayerState> FallState::moveRight() {
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    auto spriteComponent = m_playerComponent->getSpriteComponent();
    // 如果当前速度是向左的，则先减速到0 (增强操控手感)
    if (physicsComponent->m_velocity.x < 0.0f) physicsComponent->m_velocity.x = 0.0f;
    physicsComponent->addForce({m_playerComponent->getMoveForce(), 0.0f});
    spriteComponent->setFlipped(false);

    return nullptr;
}

std::unique_ptr<PlayerState> FallState::climbUp() {
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    // 如果与梯子重合，则切换到 ClimbState
    if (physicsComponent->hasCollidedLadder()) {
        return std::make_unique<ClimbState>(m_playerComponent);
    }

    return nullptr;
}

std::unique_ptr<PlayerState> FallState::climbDown() {
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    // 如果与梯子重合，则切换到 ClimbState
    if (physicsComponent->hasCollidedLadder()) {
        return std::make_unique<ClimbState>(m_playerComponent);
    }

    return nullptr;
}


} // namespace game::component::state