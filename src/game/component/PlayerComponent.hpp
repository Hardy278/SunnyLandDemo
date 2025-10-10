#pragma once
#include "../../engine/component/Component.hpp"
#include "state/PlayerState.hpp"
#include <memory>

namespace engine::input {
    class InputManager;
}
namespace engine::component {
    class TransformComponent;
    class PhysicsComponent;
    class SpriteComponent;
    class AnimationComponent;
    class HealthComponent;
    class AudioComponent;
}

namespace game::component::state {
    class PlayerState;
}

namespace game::component {

/**
 * @brief 处理玩家输入、状态和控制 GameObject 移动的组件。
 *        使用状态模式管理 Idle, Walk, Jump, Fall 等状态。
 */
class PlayerComponent final : public engine::component::Component {
    friend class engine::object::GameObject;
private:
    engine::component::TransformComponent* m_transformComponent = nullptr; // 指向 TransformComponent 的非拥有指针
    engine::component::SpriteComponent*    m_spriteComponent    = nullptr;
    engine::component::PhysicsComponent*   m_physicsComponent   = nullptr;
    engine::component::AnimationComponent* m_animationComponent = nullptr;
    engine::component::HealthComponent*    m_healthComponent    = nullptr;

    std::unique_ptr<state::PlayerState> m_currentState;
    bool m_isDead = false;

    // --- 移动相关参数
    float m_moveForce = 300.0f;         ///< @brief 水平移动力
    float m_maxSpeed = 160.0f;          ///< @brief 最大移动速度 (像素/秒)
    float m_climbSpeed = 100.0f;        ///< @brief 爬梯子速度 (像素/秒)
    float m_frictionFactor = 0.70f;     ///< @brief 摩擦系数 (Idle时缓冲效果，每帧乘以此系数)
    float m_jumpVel = 320.0f;           ///< @brief 跳跃速度 (按下"jump"键给的瞬间向上的速度)
    // --- 属性相关参数 ---
    float m_stunnedDuration = 0.4f;     ///< @brief 玩家被击中后的硬直时间（单位：秒）
    // 土狼时间(Coyote Time): 允许玩家在离地后短暂时间内仍然可以跳跃
    static constexpr float m_coyoteTime = 0.1f;     ///< @brief Coyote Time (单位：秒）
    float m_coyoteTimer = 0.0f;                     ///< @brief Coyote Time 计时器
    // 无敌闪烁时间
    static constexpr float m_flashInterval = 0.1f;  ///< @brief 闪烁间隔时间（单位：秒）
    float m_flashTimer = 0.0f;                      ///< @brief 闪烁计时器，用于无敌状态下的闪烁效果

public:
    PlayerComponent() = default;
    ~PlayerComponent() override = default;

    // 禁止拷贝和移动
    PlayerComponent(const PlayerComponent&) = delete;
    PlayerComponent& operator=(const PlayerComponent&) = delete;
    PlayerComponent(PlayerComponent&&) = delete;
    PlayerComponent& operator=(PlayerComponent&&) = delete;

    bool takeDamage(int damage);        ///< @brief 试图造成伤害，返回是否成功

    // setters and getters
    engine::component::TransformComponent* getTransformComponent() const { return m_transformComponent; }
    engine::component::SpriteComponent* getSpriteComponent() const { return m_spriteComponent; }
    engine::component::PhysicsComponent* getPhysicsComponent() const { return m_physicsComponent; }
    engine::component::AnimationComponent* getAnimationComponent() const { return m_animationComponent; }
    engine::component::HealthComponent* getHealthComponent() const { return m_healthComponent; }

    void setIsDead(bool isDead) { m_isDead = isDead; }                  ///< @brief 设置玩家是否死亡
    bool isDead() const { return m_isDead; }                            ///< @brief 获取玩家是否死亡    
    void setMoveForce(float moveForce) { m_moveForce = moveForce; }     ///< @brief 设置水平移动力
    float getMoveForce() const { return m_moveForce; }                  ///< @brief 获取水平移动力  
    void setMaxSpeed(float max_speed) { m_maxSpeed = max_speed; }       ///< @brief 设置最大移动速度
    float getMaxSpeed() const { return m_maxSpeed; }                    ///< @brief 获取最大移动速度
    void setClimbSpeed(float climbSpeed) { m_climbSpeed = climbSpeed; } ///< @brief 设置爬梯子速度
    float getClimbSpeed() const { return m_climbSpeed; }                ///< @brief 获取爬梯子速度
    void setFrictionFactor(float frictionFactor) { m_frictionFactor = frictionFactor; }   ///< @brief 设置摩擦系数
    float getFrictionFactor() const { return m_frictionFactor; }        ///< @brief 获取摩擦系数
    void setJumpVelocity(float jumpVel) { m_jumpVel = jumpVel; }        ///< @brief 设置跳跃速度
    float getJumpVelocity() const { return m_jumpVel; }                 ///< @brief 获取跳跃速度
    void setStunnedDuration(float duration) { m_stunnedDuration = duration; }  ///< @brief 设置硬直时间
    float getStunnedDuration() const { return m_stunnedDuration; }       ///< @brief 获取硬直时间

    void setState(std::unique_ptr<state::PlayerState> new_state);       ///< @brief 切换玩家状态
    bool isOnGround() const;                              ///< @brief 检查玩家是否在地面上(考虑了Coyote Time)
    
private:
    // 核心循环函数
    void init() override;
    void handleInput(engine::core::Context& context) override;
    void update(float deltaTime, engine::core::Context& context) override;
    
};

} // namespace game::component