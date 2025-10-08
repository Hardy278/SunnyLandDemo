#pragma once
#include "Component.hpp"

#include <glm/vec2.hpp>
#include <utility>

namespace engine::physics {
    class PhysicsEngine;
}

namespace engine::component {
class TransformComponent;

/**
 * @brief 管理GameObject的物理属性
 *
 * 存储速度、质量、力和重力设置。与PhysicsEngine交互。
 */
class PhysicsComponent final: public Component {
    friend class engine::object::GameObject;
public:
    glm::vec2 m_velocity = {0.0f, 0.0f};             ///< @brief 物体的速度，设为公共成员变量，方便PhysicsEngine访问更新

private:
    engine::physics::PhysicsEngine* m_physicsEngine = nullptr;  ///< @brief 指向PhysicsEngine的指针
    TransformComponent* m_transform = nullptr;                   ///< @brief TransformComponent的缓存指针

    glm::vec2 m_force = {0.0f, 0.0f};                            ///< @brief 当前帧受到的力
    float m_mass = 1.0f;                             ///< @brief 物体质量（默认1.0）
    bool m_useGravity = true;                       ///< @brief 物体是否受重力影响
    bool m_enabled = true;                           ///< @brief 组件是否激活

    // --- 碰撞状态标志 ---
    bool m_collidedBelow = false;
    bool m_collidedAbove = false;
    bool m_collidedLeft = false;
    bool m_collidedRight = false;
    bool m_collidedLadder = false;      ///< @brief 是否与梯子发生碰撞
    bool m_isOnTopLadder = false;     ///< @brief 是否在梯子顶层（梯子上方没有瓦片）

public:
    /**
     * @brief 构造函数  
     * 
     * @param physicsEngine 指向PhysicsEngine的指针，不能为nullptr
     * @param useGravity 物体是否受重力影响，默认true
     * @param mass 物体质量，默认1.0
     */
    PhysicsComponent(engine::physics::PhysicsEngine* physicsEngine, bool useGravity = true, float mass = 1.0f);
    ~PhysicsComponent() override = default;

    // 删除复制/移动操作
    PhysicsComponent(const PhysicsComponent&) = delete;
    PhysicsComponent& operator=(const PhysicsComponent&) = delete;
    PhysicsComponent(PhysicsComponent&&) = delete;
    PhysicsComponent& operator=(PhysicsComponent&&) = delete;

    // PhysicsEngine使用的物理方法
    void addForce(const glm::vec2& force) { if (m_enabled) m_force += force; }    ///< @brief 添加力
    void clearForce() { m_force = {0.0f, 0.0f}; }                                ///< @brief 清除力
    const glm::vec2& getForce() const { return m_force; }                        ///< @brief 获取当前力
    float getMass() const { return m_mass; }                                     ///< @brief 获取质量
    bool isEnabled() const { return m_enabled; }                                 ///< @brief 获取组件是否启用
    bool isUseGravity() const { return m_useGravity; }                          ///< @brief 获取组件是否受重力影响

    // 设置器/获取器
    void setEnabled(bool enabled) { m_enabled = enabled; }                       ///< @brief 设置组件是否启用
    void setMass(float mass) { m_mass = (mass >= 0.0f) ? mass : 1.0f; }          ///< @brief 设置质量，质量不能为负
    void setUseGravity(bool useGravity) { m_useGravity = useGravity; }        ///< @brief 设置组件是否受重力影响
    void setVelocity(glm::vec2 velocity) { m_velocity = std::move(velocity); }       ///< @brief 设置速度
    const glm::vec2& getVelocity() const { return m_velocity; }                  ///< @brief 获取当前速度
    TransformComponent* getTransform() const { return m_transform; }             ///< @brief 获取TransformComponent指针

    // --- 碰撞状态访问与修改 (供 PhysicsEngine 使用) ---
    /** @brief 重置所有碰撞标志 (在物理更新开始时调用) */
    void resetCollisionFlags() {
        m_collidedBelow = false;
        m_collidedAbove = false;
        m_collidedLeft = false;
        m_collidedRight = false;
        m_collidedLadder = false;
        m_isOnTopLadder = false;
    }

    void setCollidedBelow(bool collided) { m_collidedBelow = collided; }    ///< @brief 设置下方碰撞标志
    void setCollidedAbove(bool collided) { m_collidedAbove = collided; }    ///< @brief 设置上方碰撞标志
    void setCollidedLeft(bool collided) { m_collidedLeft = collided; }      ///< @brief 设置左方碰撞标志
    void setCollidedRight(bool collided) { m_collidedRight = collided; }    ///< @brief 设置右方碰撞标志
    void setCollidedLadder(bool collided) { m_collidedLadder = collided; }  ///< @brief 设置梯子碰撞标志
    void setOnTopLadder(bool onTop) { m_isOnTopLadder = onTop; }        ///< @brief 设置是否在梯子顶层

    bool hasCollidedBelow() const { return m_collidedBelow; }       ///< @brief 检查是否与下方发生碰撞
    bool hasCollidedAbove() const { return m_collidedAbove; }       ///< @brief 检查是否与上方发生碰撞
    bool hasCollidedLeft() const { return m_collidedLeft; }         ///< @brief 检查是否与左方发生碰撞
    bool hasCollidedRight() const { return m_collidedRight; }       ///< @brief 检查是否与右方发生碰撞
    bool hasCollidedLadder() const { return m_collidedLadder; }     ///< @brief 检查是否与梯子发生碰撞
    bool isOnTopLadder() const { return m_isOnTopLadder; }        ///< @brief 检查是否在梯子顶层

private:
    // 核心循环方法
    void init() override;
    void update(float, engine::core::Context&) override {}
    void clean() override;
};

} // namespace engine::component