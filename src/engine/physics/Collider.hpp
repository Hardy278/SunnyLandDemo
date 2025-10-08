#pragma once
#include <glm/vec2.hpp>
#include <utility>

namespace engine::physics {

/**
 * @brief 定义不同类型的碰撞器。
 */
enum class ColliderType {
    NONE,
    AABB,
    CIRCLE,
    // 未来可能添加其他类型，如 Capsule, Polygon 等
};

/**
 * @brief 碰撞器的抽象基类。
 * 所有具体的碰撞器都应继承此类。
 */
class Collider {
protected:
    glm::vec2 m_aabbSize = {0.0f, 0.0f};    ///< @brief 覆盖Collider的最小包围盒的尺寸（宽度和高度）。

public:
    virtual ~Collider() = default;
    virtual ColliderType getType() const = 0;   ///< @brief 获取碰撞器的类型。

    void setAABBSize(glm::vec2 size) { m_aabbSize = std::move(size); }  ///< @brief 设置最小包围盒的尺寸（宽度和高度）。
    const glm::vec2& getAABBSize() const { return m_aabbSize; }     ///< @brief 获取最小包围盒的尺寸（宽度和高度）。
};

/**
 * @brief 轴对齐包围盒 (Axis-Aligned Bounding Box) 碰撞器。
 */
class AABBCollider final : public Collider {
private:
    glm::vec2 m_size = {0.0f, 0.0f};  ///< @brief 包围盒的尺寸（和m_aabbSize相同）。

public:
    /**
     * @brief 构造函数。
     * @param size 包围盒的宽度和高度。
     */
    explicit AABBCollider(glm::vec2 size) : m_size(std::move(size)) { setAABBSize(m_size); }
    ~AABBCollider() override = default;

    // --- Getters and Setters ---
    ColliderType getType() const override { return ColliderType::AABB; }
    const glm::vec2& getSize() const { return m_size; }
    void setSize(glm::vec2 size) { m_size = std::move(size); }
};

/**
 * @brief 圆形碰撞器。
 */
class CircleCollider final : public Collider {
private:
    float m_radius = 0.0f;  ///< @brief 圆的半径。

public:
    /**
     * @brief 构造函数。
     * @param radius 圆的半径。
     */
    explicit CircleCollider(float radius) : m_radius(radius) { setAABBSize(glm::vec2(radius * 2.0f, radius * 2.0f)); }
    ~CircleCollider() override = default;

    // --- Getters and Setters ---
    ColliderType getType() const override { return ColliderType::CIRCLE; }
    float getRadius() const { return m_radius; }
    void setRadius(float radius) { m_radius = radius; }
};

} // namespace engine::physics