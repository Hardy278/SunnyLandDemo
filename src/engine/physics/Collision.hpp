#pragma once
#include "../utils/Math.hpp"

namespace engine::component {
class ColliderComponent;
}

namespace engine::physics::collision {

/**
 * @brief 检查两个碰撞器组件是否重叠。
 * @param a 第一个碰撞器组件。
 * @param b 第二个碰撞器组件。
 * @return true 如果碰撞器组件重叠，否则为 false。
 */
bool checkCollision(const engine::component::ColliderComponent& a, const engine::component::ColliderComponent& b);

/**
 * @brief 检查两个圆形是否重叠。
 * 
 * @param aCenter 第一个圆的中心。
 * @param aRadius 第一个圆的半径。
 * @param bCenter 第二个圆的中心。
 * @param bRadius 第二个圆的半径。
 * @return true 如果两个圆重叠，否则为 false。
 */
bool checkCircleOverlap(const glm::vec2& aCenter, const float aRadius, const glm::vec2& bCenter, const float bRadius);

/**
 * @brief 检查两个轴对齐包围盒 (AABB) 是否重叠。
 * @param aPos 第一个AABB的左上角坐标。
 * @param aSize 第一个AABB的尺寸。
 * @param bPos 第二个AABB的左上角坐标。
 * @param bSize 第二个AABB的尺寸。
 * @return true 如果AABB重叠，否则为 false。
 */
bool checkAABBOverlap(const glm::vec2& aPos, const glm::vec2& aSize, const glm::vec2& bPos, const glm::vec2& bSize);

/**
 * @brief 检查两个矩形是否重叠。
 * 
 * @param a 第一个矩形。
 * @param b 第二个矩形。
 * @return true 如果矩形重叠，否则为 false。
 */
bool checkRectOverlap(const engine::utils::Rect& a, const engine::utils::Rect& b);

/**
 * @brief 检查一个点是否在圆内。
 * 
 * @param point 要检查的点。
 * @param center 圆的中心。
 * @param radius 圆的半径。
 * @return true 如果点在圆内，否则为 false。
 */
bool checkPointInCircle(const glm::vec2& point, const glm::vec2& center, const float radius);

// 未来可以添加更多碰撞检测相关的函数，

} // namespace engine::physics::collision