#include "PlayerComponent.hpp"
#include "state/IdleState.hpp"
#include "state/HurtState.hpp"
#include "state/DeadState.hpp"
#include "../../engine/component/TransformComponent.hpp"
#include "../../engine/component/PhysicsComponent.hpp"
#include "../../engine/component/SpriteComponent.hpp"
#include "../../engine/component/AnimationComponent.hpp"
#include "../../engine/component/HealthComponent.hpp"
#include "../../engine/object/GameObject.hpp"
#include "../../engine/input/InputManager.hpp"

#include <spdlog/spdlog.h>
#include <glm/common.hpp>

#include <utility>
#include <typeinfo>

namespace game::component {

void PlayerComponent::init() {
    if (!m_owner) {
        spdlog::error("PLAYERCOMPONENT::init::ERROR::PlayerComponent 没有所属游戏对象!");
        return;
    }

    // 获取必要的组件
    m_transformComponent = m_owner->getComponent<engine::component::TransformComponent>();
    m_physicsComponent = m_owner->getComponent<engine::component::PhysicsComponent>();
    m_spriteComponent = m_owner->getComponent<engine::component::SpriteComponent>();
    m_animationComponent = m_owner->getComponent<engine::component::AnimationComponent>();
    m_healthComponent = m_owner->getComponent<engine::component::HealthComponent>();

    // 检查必要组件是否存在
    if (!m_transformComponent || !m_physicsComponent || !m_spriteComponent || !m_animationComponent || !m_healthComponent) {
        spdlog::error("PLAYERCOMPONENT::init::ERROR::Player 对象缺少必要组件！");
        return;
    }

    // 初始化状态机
    m_currentState = std::make_unique<state::IdleState>(this);
    if (m_currentState) {
        setState(std::move(m_currentState));
    } else {
        spdlog::error("PLAYERCOMPONENT::init::ERROR::初始化玩家状态失败 ( make_unique 返回空指针) !");
    }
    spdlog::debug("PLAYERCOMPONENT::init::DEBUG::PlayerComponent 初始化完成。");
}

bool PlayerComponent::takeDamage(int damage) {
    if (m_isDead || !m_healthComponent || damage <= 0) {
        spdlog::warn("PLAYERCOMPONENT::takeDamage::WARN::玩家已死亡或却少必要组件，并未造成伤害。");
        return false;
    }

    bool success = m_healthComponent->takeDamage(damage);
    if (!success) return false;
    // --- 成功造成伤害了，根据是否存活决定状态切换
    if (m_healthComponent->isAlive()) {
        spdlog::debug("PLAYERCOMPONENT::takeDamage::DEBUG::玩家受到了 {} 点伤害，当前生命值: {}/{}。",damage, m_healthComponent->getCurrentHealth(), m_healthComponent->getMaxHealth());
        // 切换到受伤状态
        setState(std::make_unique<state::HurtState>(this));
    } else {
        spdlog::debug("PLAYERCOMPONENT::takeDamage::DEBUG::玩家死亡。");
        m_isDead = true;
        // 切换到死亡状态
        setState(std::make_unique<state::DeadState>(this));
    }
    return true;
}

void PlayerComponent::setState(std::unique_ptr<state::PlayerState> newState) {
    if (!newState) {
        spdlog::warn("PLAYERCOMPONENT::setState::WARN::尝试设置空的玩家状态！");
        return;
    }
    if (m_currentState) {
        spdlog::debug("PLAYERCOMPONENT::setState::DEBUG::从状态 {} 切换到 {}", typeid(*m_currentState).name(), typeid(*newState).name());
        m_currentState->exit();
    }
    m_currentState = std::move(newState);
    m_currentState->enter();
}

bool PlayerComponent::isOnGround() const {
    bool onGround = m_coyoteTimer <= m_coyoteTime || m_physicsComponent->hasCollidedBelow();
    return onGround;
}

void PlayerComponent::handleInput(engine::core::Context& context) {
    if (!m_currentState) return;
    auto nextState = m_currentState->handleInput(context);
    if (nextState) {
        setState(std::move(nextState));
    }
}

void PlayerComponent::update(float deltaTime, engine::core::Context& context) {
    if (!m_currentState) return;

    // 一旦离地，开始计时 Coyote Timer
    if (!m_physicsComponent->hasCollidedBelow()) {
        m_coyoteTimer += deltaTime;
    } else {    // 如果碰撞到地面，重置 Coyote Timer
        m_coyoteTimer = 0.0f; 
    }

    auto nextState = m_currentState->update(deltaTime, context);
    if (nextState) {
        setState(std::move(nextState));
    }
}

} // namespace game::component 