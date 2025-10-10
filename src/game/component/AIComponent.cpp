#include "AIComponent.hpp"
#include "ai/AIBehavior.hpp"
#include "../../engine/object/GameObject.hpp"
#include "../../engine/component/TransformComponent.hpp"
#include "../../engine/component/PhysicsComponent.hpp"
#include "../../engine/component/SpriteComponent.hpp"
#include "../../engine/component/AnimationComponent.hpp"
#include "../../engine/component/HealthComponent.hpp"
#include <spdlog/spdlog.h>

namespace game::component {

void AIComponent::init() {
    if (!m_owner) {
        spdlog::error("PlayerComponent 没有所属游戏对象!");
        return;
    }

    // 获取并缓存必要的组件指针
    m_transformComponent = m_owner->getComponent<engine::component::TransformComponent>();
    m_physicsComponent = m_owner->getComponent<engine::component::PhysicsComponent>();
    m_spriteComponent = m_owner->getComponent<engine::component::SpriteComponent>();
    m_animationComponent = m_owner->getComponent<engine::component::AnimationComponent>();

    // 检查是否所有必需的组件都存在(音频组件并非必须存在)
    if (!m_transformComponent || !m_physicsComponent || !m_spriteComponent || !m_animationComponent) {
        spdlog::error("GameObject '{}' 上的 AIComponent 缺少必需的组件", m_owner->getName());
    }
}

void AIComponent::update(float deltaTime, engine::core::Context&) {
    // 将更新委托给当前的行为策略
    if (m_currentBehavior) {
        m_currentBehavior->update(deltaTime, *this);
    } else {
        spdlog::warn("GameObject '{}' 上的 AIComponent 没有设置行为。", m_owner ? m_owner->getName() : "Unknown");
    }
}

void AIComponent::setBehavior(std::unique_ptr<ai::AIBehavior> behavior) {
    m_currentBehavior = std::move(behavior);
    spdlog::debug("GameObject '{}' 上的 AIComponent 设置了新的行为。", m_owner ? m_owner->getName() : "Unknown");
    if (m_currentBehavior) {
        m_currentBehavior->enter(*this); // 调用新行为的 enter 方法
    }
}

bool AIComponent::takeDamage(int damage) {
    bool success = false;
    if (auto* healthComponent = getOwner()->getComponent<engine::component::HealthComponent>(); healthComponent) {
        success = healthComponent->takeDamage(damage);
        // TODO: 可以设置受伤/死亡后的行为
    }
    return success;
}

bool AIComponent::isAlive() const {
    if (auto* healthComponent = getOwner()->getComponent<engine::component::HealthComponent>(); healthComponent) {
        return healthComponent->isAlive();
    }
    return true;    // 如果没有生命组件，默认返回存活状态
}

} // namespace game::component
