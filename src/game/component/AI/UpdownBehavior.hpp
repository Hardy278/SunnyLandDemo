#pragma once
#include "AIBehavior.hpp"

namespace game::component::ai {

/**
 * @brief AI 行为：在指定范围内上下垂直移动。
 *
 * 到达边界或碰到障碍物时会反向。
 */
class UpDownBehavior final : public AIBehavior {
    friend class game::component::AIComponent;
private:
    float m_patrolMinY = 0.0f;     ///< @brief 巡逻范围的上边界 (Y 坐标较小值)
    float m_patrolMaxY = 0.0f;     ///< @brief 巡逻范围的下边界 (Y 坐标较大值)
    float m_moveSpeed = 50.0f;      ///< @brief 移动速度 (像素/秒)
    bool m_movingDown = false;      ///< @brief 当前是否向下移动

public:
    /**
     * @brief 构造函数。
     * @param minY 巡逻范围的最小 y 坐标。
     * @param maxY 巡逻范围的最大 y 坐标。
     * @param speed 移动速度。
     */
    UpDownBehavior(float minY, float maxY, float speed = 50.0f);
    ~UpDownBehavior() override = default;

    // 禁止拷贝和移动
    UpDownBehavior(const UpDownBehavior&) = delete;
    UpDownBehavior& operator=(const UpDownBehavior&) = delete;
    UpDownBehavior(UpDownBehavior&&) = delete;
    UpDownBehavior& operator=(UpDownBehavior&&) = delete;

private:
    void enter(AIComponent& AIComponent) override;
    void update(float deltaTime, AIComponent& AIComponent) override;
};

} // namespace game::component::ai