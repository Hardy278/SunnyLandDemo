#pragma once
#include "AIBehavior.hpp"

namespace game::component::ai {

/**
 * @brief AI 行为：在指定范围内左右巡逻。
 *
 * 遇到墙壁或到达巡逻边界时会转身。
 */
class PatrolBehavior final : public AIBehavior {
    friend class game::component::AIComponent;
private:
    float m_patrolMinX = 0.0f;      ///< @brief 巡逻范围的左边界
    float m_patrolMaxX = 0.0f;      ///< @brief 巡逻范围的右边界
    float m_moveSpeed = 50.0f;      ///< @brief 移动速度 (像素/秒)
    bool m_movingRight = false;     ///< @brief 当前是否向右移动

public:
    /**
     * @brief 构造函数。
     * @param minX 巡逻范围的最小 x 坐标。
     * @param maxX 巡逻范围的最大 x 坐标。
     * @param speed 移动速度。
     */
    PatrolBehavior(float minX, float maxX, float speed = 50.0f);
    ~PatrolBehavior() override = default;

    // 禁止拷贝和移动
    PatrolBehavior(const PatrolBehavior&) = delete;
    PatrolBehavior& operator=(const PatrolBehavior&) = delete;
    PatrolBehavior(PatrolBehavior&&) = delete;
    PatrolBehavior& operator=(PatrolBehavior&&) = delete;

private:
    void enter(AIComponent& AIComponent) override;
    void update(float deltaTime, AIComponent& AIComponent) override;
};

} // namespace game::component::ai