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
}

void WalkState::exit() {
}

std::unique_ptr<PlayerState> WalkState::handleInput(engine::core::Context& context) {
    auto inputManager = context.getInputManager();
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    auto spriteComponent = m_playerComponent->getSpriteComponent();
    // 如果按"move_up"键，且与梯子重合，则切换到 ClimbState
    if (physicsComponent->hasCollidedLadder() && inputManager.isActionDown("move_up")) {
        return std::make_unique<ClimbState>(m_playerComponent);
    }
    // 如果按下“jump”则切换到 JumpState
    if (inputManager.isActionPressed("jump")) {
        return std::make_unique<JumpState>(m_playerComponent);
    }
    // 步行状态可以左右移动
    if (inputManager.isActionDown("move_left")) {
        if (physicsComponent->m_velocity.x > 0.0f) {
            physicsComponent->m_velocity.x = 0.0f;  // 如果当前速度是向右的，则先减速到0 (增强操控手感)
        }
        // 添加向左的水平力
        physicsComponent->addForce({-m_playerComponent->getMoveForce(), 0.0f});
        spriteComponent->setFlipped(true);         // 向左移动时翻转
    } else if (inputManager.isActionDown("move_right")) {
        if (physicsComponent->m_velocity.x < 0.0f) {
            physicsComponent->m_velocity.x = 0.0f;  // 如果当前速度是向左的，则先减速到0
        }
        // 添加向右的水平力
        physicsComponent->addForce({m_playerComponent->getMoveForce(), 0.0f});
        spriteComponent->setFlipped(false);        // 向右移动时不翻转
    } else {
        // 如果没有按下左右移动键，则切换到 IdleState
        return std::make_unique<IdleState>(m_playerComponent);
    }
    return nullptr;
}

std::unique_ptr<PlayerState> WalkState::update(float, engine::core::Context&) {
    // 限制最大速度
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    auto maxSpeed = m_playerComponent->getMaxSpeed();
    physicsComponent->m_velocity.x = glm::clamp(physicsComponent->m_velocity.x, -maxSpeed, maxSpeed);
    // 如果离地，则切换到 FallState
    if (!m_playerComponent->isOnGround()) {
        return std::make_unique<FallState>(m_playerComponent);
    }
    return nullptr;
}

} // namespace game::component::state