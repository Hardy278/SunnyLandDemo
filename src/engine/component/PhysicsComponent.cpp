#include "PhysicsComponent.hpp"
#include "TransformComponent.hpp"
#include "../object/GameObject.hpp"
#include "../physics/PhysicsEngine.hpp"
#include <spdlog/spdlog.h>

namespace engine::component {

PhysicsComponent::PhysicsComponent(engine::physics::PhysicsEngine* physicsEngine, bool useGravity, float mass)
    : m_physicsEngine(physicsEngine), m_mass(mass >= 0.0f ? mass : 1.0f), m_useGravity(useGravity) {
    if (!m_physicsEngine) {
        spdlog::error("PhysicsComponent构造函数中, PhysicsEngine指针不能为nullptr!");
    }
    spdlog::trace("物理组件创建完成，质量: {}, 使用重力: {}", m_mass, m_useGravity);
}

void PhysicsComponent::init() {
    if (!m_owner) {
        spdlog::error("物理组件初始化前需要一个GameObject作为所有者!");
        return;
    }
    if (!m_physicsEngine) {
        spdlog::error("物理组件初始化时, PhysicsEngine未正确初始化");
        return;
    }
    m_transform = m_owner->getComponent<TransformComponent>();
    if (!m_transform) {
        spdlog::warn("物理组件初始化时, 同一GameObject上没有找到TransformComponent组件。");
    }
    // 注册到PhysicsEngine
    m_physicsEngine->registerComponent(this);
    spdlog::trace("物理组件初始化完成。");
}

void PhysicsComponent::clean() {
    m_physicsEngine->unregisterComponent(this);
    spdlog::trace("物理组件清理完成。");
}

} // namespace engine::component