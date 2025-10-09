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

std::unique_ptr<PlayerState> FallState::handleInput(engine::core::Context& context) {
    auto inputManager = context.getInputManager();
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    auto spriteComponent = m_playerComponent->getSpriteComponent();
    // 如果按下上下键，且与梯子重合，则切换到 ClimbState
    if (physicsComponent->hasCollidedLadder() &&
        (inputManager.isActionDown("move_up") || inputManager.isActionDown("move_down"))) {
        return std::make_unique<ClimbState>(m_playerComponent);
    }
    // 下落状态下可以左右移动
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

std::unique_ptr<PlayerState> FallState::update(float, engine::core::Context&) {
    // 限制最大速度(水平方向)
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    auto max_speed = m_playerComponent->getMaxSpeed();
    physicsComponent->m_velocity.x = glm::clamp(physicsComponent->m_velocity.x, -max_speed, max_speed);

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

} // namespace game::component::state