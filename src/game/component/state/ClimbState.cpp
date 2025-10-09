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

std::unique_ptr<PlayerState> ClimbState::handleInput(engine::core::Context& context) {
    
    auto inputManager = context.getInputManager();
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    auto animationComponent = m_playerComponent->getAnimationComponent();

    // --- 攀爬状态下，按键则移动，不按键则静止 ---
    auto isUp = inputManager.isActionDown("move_up");
    auto isDown = inputManager.isActionDown("move_down");
    auto isLeft = inputManager.isActionDown("move_left");
    auto isRight = inputManager.isActionDown("move_right");
    auto speed = m_playerComponent->getClimbSpeed();
    // 三目运算符嵌套，自左向右执行
    physicsComponent->m_velocity.y = isUp ? -speed : isDown ? speed : 0.0f;
    physicsComponent->m_velocity.x = isLeft ? -speed : isRight ? speed : 0.0f;

    // --- 根据是否有按键决定动画播放情况 ---
    // 有按键则恢复动画播放
    // 无按键则停止动画播放
    (isUp || isDown || isLeft || isRight) ? animationComponent->resumeAnimation() : animationComponent->stopAnimation();     
    
    // 按跳跃键主动离开攀爬状态
    if (inputManager.isActionPressed("jump")) {
        return std::make_unique<JumpState>(m_playerComponent);
    }
    return nullptr;
}

std::unique_ptr<PlayerState> ClimbState::update(float, engine::core::Context&) {
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    // 如果着地，则切换到 IdleState
    if (physicsComponent->hasCollidedBelow()) {
        return std::make_unique<IdleState>(m_playerComponent);
    }
    // 如果离开梯子区域，则切换到 FallState（能走到这里 说明非着地状态）
    if (!physicsComponent->hasCollidedLadder()) {
        return std::make_unique<FallState>(m_playerComponent);
    }
    return nullptr;
}


} // namespace game::component::state