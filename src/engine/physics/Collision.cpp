#include "Collision.hpp"
#include "../component/ColliderComponent.hpp"
#include "../component/TransformComponent.hpp"

namespace engine::physics::collision {

bool checkCollision(const engine::component::ColliderComponent& a, const engine::component::ColliderComponent& b) {
    // 获取两个碰撞盒及对应Transform信息
    auto aCollider = a.getCollider();
    auto bCollider = b.getCollider();
    auto aTransform = a.getTransform();
    auto bTransform = b.getTransform();

    // 先计算最小包围盒是否碰撞，如果没有碰撞，那一定是返回false (不考虑AABB的旋转)
    auto aSize = aCollider->getAABBSize() * aTransform->getScale();
    auto bSize = bCollider->getAABBSize() * bTransform->getScale();
    auto aPos = aTransform->getPosition() + a.getOffset();
    auto bPos = bTransform->getPosition() + b.getOffset();
    if (!checkAABBOverlap(aPos, aSize, bPos, bSize)) {
        return false;
    }

    // --- 如果最小包围盒有碰撞，再进行更细致的判断 ---
    // AABB vs AABB, 直接返回真
    if (aCollider->getType() == engine::physics::ColliderType::AABB && bCollider->getType() == engine::physics::ColliderType::AABB) {
        return true;
    } else if (aCollider->getType() == engine::physics::ColliderType::CIRCLE && bCollider->getType() == engine::physics::ColliderType::CIRCLE) {
        // Circle vs Circle: 判断两个圆心距离是否小于两个圆的半径之和
        auto aCenter = aPos + 0.5f * aSize;  // 圆心位置
        auto bCenter = bPos + 0.5f * bSize;
        auto aRadius = 0.5f * aSize.x;        // 圆的半径等于AABB的一半宽度
        auto bRadius = 0.5f * bSize.x;
        return checkCircleOverlap(aCenter, aRadius, bCenter, bRadius);
    } else if (aCollider->getType() == engine::physics::ColliderType::AABB && bCollider->getType() == engine::physics::ColliderType::CIRCLE){
        // AABB vs Circle: 判断圆心到AABB的最邻近点是否在圆内
        auto bCenter = bPos + 0.5f * bSize;
        auto bRadius = 0.5f * bSize.x;
        auto nearestPoint = glm::clamp(bCenter, aPos, aPos + aSize);   // 计算圆心到AABB的最邻近点
        return checkPointInCircle(nearestPoint, bCenter, bRadius);
    } else if (aCollider->getType() == engine::physics::ColliderType::CIRCLE && bCollider->getType() == engine::physics::ColliderType::AABB){
        // Circle vs AABB
        auto aCenter = aPos + 0.5f * aSize;
        auto aRadius = 0.5f * aSize.x;
        auto nearestPoint = glm::clamp(aCenter, bPos, bPos + bSize);   // 计算圆心到AABB的最邻近点
        return checkPointInCircle(nearestPoint, aCenter, aRadius);
    }
    return false;
}

bool checkCircleOverlap(const glm::vec2& aCenter, const float aRadius, const glm::vec2& bCenter, const float bRadius) {
    return (glm::length(aCenter - bCenter) < aRadius + bRadius);
}

bool checkAABBOverlap(const glm::vec2& aPos, const glm::vec2& aSize, const glm::vec2& bPos, const glm::vec2& bSize) {
    // 检查两个AABB是否重叠
    if (aPos.x + aSize.x <= bPos.x || aPos.x >= bPos.x + bSize.x ||
        aPos.y + aSize.y <= bPos.y || aPos.y >= bPos.y + bSize.y) {
        return false;
    }
    return true;
}

bool checkRectOverlap(const engine::utils::Rect& a, const engine::utils::Rect& b) {
    return checkAABBOverlap(a.position, a.size, b.position, b.size);
}

bool checkPointInCircle(const glm::vec2 &point, const glm::vec2 &center, const float radius) {
    return (glm::length(point - center) < radius);
}

} // namespace engine::physics::collision 