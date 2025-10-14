#include "HealthComponent.hpp"
#include "../../engine/object/GameObject.hpp"
#include <spdlog/spdlog.h>
#include <glm/common.hpp>

namespace engine::component {

HealthComponent::HealthComponent(int maxHealth, float invincibilityDuration)
    : m_maxHealth(glm::max(1, maxHealth)), // 确保最大生命值至少为 1
      m_currentHealth(m_maxHealth),         // 初始化当前生命值为最大生命值
      m_invincibilityDuration(invincibilityDuration) {}

void HealthComponent::update(float deltaTime, engine::core::Context&) {
    // 更新无敌状态计时器
    if (m_isInvincible) {
        m_invincibilityTimer -= deltaTime;
        if (m_invincibilityTimer <= 0.0f) {
            m_isInvincible = false;
            m_invincibilityTimer = 0.0f;
        }
    }
}

bool HealthComponent::takeDamage(int damageAmount) {
    if (damageAmount <= 0 || !isAlive()) {
        return false; // 不造成伤害或已经死亡
    }
    if (m_isInvincible) {
        spdlog::debug("HEALTHCOMPONENT::takeDamage::游戏对象 '{}' 处于无敌状态，免疫了 {} 点伤害。", m_owner ? m_owner->getName() : "Unknown", damageAmount);
        return false; // 无敌状态，不受伤
    }
    // --- 确实造成伤害了 ---
    m_currentHealth -= damageAmount;
    m_currentHealth = glm::max(0, m_currentHealth); // 防止生命值变为负数
    // 如果受伤但没死，并且设置了无敌时间，则触发无敌
    if (isAlive() && m_invincibilityDuration > 0.0f) {
        setInvincible(m_invincibilityDuration);
    }
    spdlog::debug("HEALTHCOMPONENT::takeDamage::游戏对象 '{}' 受到了 {} 点伤害，当前生命值: {}/{}。", m_owner ? m_owner->getName() : "Unknown", damageAmount, m_currentHealth, m_maxHealth);
    return true;        // 造成伤害，返回true
}

int HealthComponent::heal(int healAmount) {
    if (healAmount <= 0 || !isAlive()) {
        return m_currentHealth; // 不治疗或已经死亡
    }
    m_currentHealth += healAmount;
    m_currentHealth = std::min(m_maxHealth, m_currentHealth); // 防止超过最大生命值
    spdlog::debug("HEALTHCOMPONENT::heal::游戏对象 '{}' 治疗了 {} 点，当前生命值: {}/{}。", m_owner ? m_owner->getName() : "Unknown", healAmount, m_currentHealth, m_maxHealth);
    return m_currentHealth;
}

void HealthComponent::setInvincible(float duration) {
    if (duration > 0.0f) {
        m_isInvincible = true;
        m_invincibilityTimer = duration;
        spdlog::debug("HEALTHCOMPONENT::setInvincible::游戏对象 '{}' 进入无敌状态，持续 {} 秒。", m_owner ? m_owner->getName() : "Unknown", duration);
    } else {
        // 如果持续时间为 0 或负数，则立即取消无敌
        m_isInvincible = false;
        m_invincibilityTimer = 0.0f;
        spdlog::debug("HEALTHCOMPONENT::setInvincible::游戏对象 '{}' 的无敌状态被手动移除。", m_owner ? m_owner->getName() : "Unknown");
    }
}

void HealthComponent::setMaxHealth(int maxHealth) {
    m_maxHealth = glm::max(1, maxHealth); // 确保最大生命值至少为 1
    m_currentHealth = glm::min(m_currentHealth, m_maxHealth); // 确保当前生命值不超过最大生命值
}

void HealthComponent::setCurrentHealth(int currentHealth) {
    // 确保当前生命值在 0 到最大生命值之间
    m_currentHealth = glm::max(0, glm::min(currentHealth, m_maxHealth));
}

} // namespace engine::component