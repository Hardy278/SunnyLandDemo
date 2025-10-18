#include "WalkState.hpp"
#include "JumpState.hpp"
#include "FallState.hpp"
#include "IdleState.hpp"
#include "ClimbState.hpp"
#include "../PlayerComponent.hpp"
#include "../../../engine/core/Context.hpp"
#include "../../../engine/input/InputManager.hpp"
#include "../../../engine/object/GameObject.hpp"
#include "../../../engine/component/PhysicsComponent.hpp"
#include "../../../engine/component/SpriteComponent.hpp"
#include <glm/common.hpp>


namespace game::component::state {

void WalkState::enter() {
    playAnimation("walk");
    m_isMoving = true;
}

void WalkState::exit() {
}

std::unique_ptr<PlayerState> WalkState::update(float, engine::core::Context&)
{
    // 限制最大速度
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    auto maxSpeed = m_playerComponent->getMaxSpeed();
    physicsComponent->m_velocity.x = glm::clamp(physicsComponent->m_velocity.x, -maxSpeed, maxSpeed);

    // 如果没有移动，则切换到 IdleState
    if (!m_isMoving) {
        return std::make_unique<IdleState>(m_playerComponent);
    }

    // 如果离地，则切换到 FallState
    if (!m_playerComponent->isOnGround()) {
        return std::make_unique<FallState>(m_playerComponent);
    }

    m_isMoving = false;         // 循环的最后重置移动标志
    return nullptr;
}

std::unique_ptr<PlayerState> WalkState::moveLeft() {
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    auto spriteComponent = m_playerComponent->getSpriteComponent();

    if (physicsComponent->m_velocity.x > 0.0f) {
        physicsComponent->m_velocity.x = 0.0f;  // 如果当前速度是向右的，则先减速到0 (增强操控手感)
    }
    // 添加向左的水平力
    physicsComponent->addForce({-m_playerComponent->getMoveForce(), 0.0f});
    spriteComponent->setFlipped(true);         // 向左移动时翻转

    m_isMoving = true;
    return nullptr;
}

std::unique_ptr<PlayerState> WalkState::moveRight() {
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    auto spriteComponent = m_playerComponent->getSpriteComponent();

    if (physicsComponent->m_velocity.x < 0.0f) {
        physicsComponent->m_velocity.x = 0.0f;  // 如果当前速度是向左的，则先减速到0
    }
    // 添加向右的水平力
    physicsComponent->addForce({m_playerComponent->getMoveForce(), 0.0f});
    spriteComponent->setFlipped(false);        // 向右移动时不翻转 

    m_isMoving = true;
    return nullptr;
}

std::unique_ptr<PlayerState> WalkState::jump() {
    // 直接切换到 JumpState
    return std::make_unique<JumpState>(m_playerComponent);
}

std::unique_ptr<PlayerState> WalkState::climbUp() {
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    // 如果与梯子重合，则切换到 ClimbState
    if (physicsComponent->hasCollidedLadder()) {
        return std::make_unique<ClimbState>(m_playerComponent);
    }
    return nullptr;
}

} // namespace game::component::state