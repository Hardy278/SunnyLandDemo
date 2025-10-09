#pragma once
#include "../../engine/component/Component.hpp"

namespace engine::component {

/**
 * @brief 管理 GameObject 的生命值，处理伤害、治疗，并提供无敌帧功能。
 */
class HealthComponent final : public engine::component::Component {
    friend class engine::object::GameObject;
private:
    int m_maxHealth = 1;                    ///< @brief 最大生命值
    int m_currentHealth = 1;                ///< @brief 当前生命值
    bool m_isInvincible = false;            ///< @brief 是否处于无敌状态
    float m_invincibilityDuration = 2.0f;   ///< @brief 受伤后无敌的总时长（秒）
    float m_invincibilityTimer = 0.0f;      ///< @brief 无敌时间计时器（秒）

public:
    /**
     * @brief 构造函数
     * @param maxHealth 最大生命值，默认为 1
     * @param invincibilityDuration 无敌状态持续时间，默认为 2.0 秒
     */
    explicit HealthComponent(int maxHealth = 1, float invincibilityDuration = 2.0f);
    ~HealthComponent() override = default;

    // 禁止拷贝和移动
    HealthComponent(const HealthComponent&) = delete;
    HealthComponent& operator=(const HealthComponent&) = delete;
    HealthComponent(HealthComponent&&) = delete;
    HealthComponent& operator=(HealthComponent&&) = delete;

    /**
     * @brief 对 GameObject 施加伤害。
     *        如果当前处于无敌状态，则伤害无效。
     *        如果成功造成伤害且设置了无敌时长，则会触发无敌帧。
     * @param damageAmount 造成的伤害量（应为正数）。
     * @return bool 如果成功造成伤害，则返回 true，否则返回 false。
     */
    bool takeDamage(int damageAmount);
    int heal(int healAmount);             ///< @brief 治疗 GameObject，增加当前生命值（不超过最大生命值）,返回治疗后生命值

    // --- Getters and Setters ---
    bool isAlive() const { return m_currentHealth > 0; }        ///< @brief 检查 GameObject 是否存活（当前生命值大于 0）。
    bool isInvincible() const { return m_isInvincible; }        ///< @brief 检查 GameObject 是否处于无敌状态。
    int getCurrentHealth() const { return m_currentHealth; }    ///< @brief 获取当前生命值。
    int getMaxHealth() const { return m_maxHealth; }            ///< @brief 获取最大生命值。

    void setCurrentHealth(int currentHealth);                  ///< @brief 设置当前生命值 (确保不超过最大生命值)。
    void setMaxHealth(int maxHealth);                          ///< @brief 设置最大生命值 (确保不小于 1)。
    void setInvincible(float duration);                         ///< @brief 设置 GameObject 进入无敌状态，持续时间为 duration 秒。
    void setInvincibilityDuration(float duration) { m_invincibilityDuration = duration; } ///< @brief 设置无敌状态持续时间。

protected:
    // 核心循环函数
    void update(float, engine::core::Context&) override;
};

} // namespace engine::component