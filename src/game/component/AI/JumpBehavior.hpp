#pragma once
#include "AIBehavior.hpp"
#include <glm/vec2.hpp>

namespace game::component::ai {

/**
 * @brief AI 行为：在指定范围内周期性地跳跃。
 *
 * 在地面时等待，然后向当前方向跳跃。
 * 撞墙或到达边界时改变下次跳跃方向。
 */
class JumpBehavior final : public AIBehavior {
    friend class game::component::AIComponent;
private:
    float m_patrolMinX = 0.0f;                         ///< @brief 巡逻范围的左边界
    float m_patrolMaxX = 0.0f;                         ///< @brief 巡逻范围的右边界
    glm::vec2 m_jumpVel = glm::vec2(100.0f, -300.0f);   ///< @brief 跳跃速度
    float m_jumpInterval = 2.0f;                        ///< @brief 跳跃间隔时间 (秒)
    float m_jumpTimer = 0.0f;                           ///< @brief 距离下次跳跃的计时器
    bool m_jumpingRight = false;                        ///< @brief 当前是否向右跳跃

public:
    /**
     * @brief 构造函数。
     * @param minX 巡逻范围的最小 x 坐标。
     * @param maxX 巡逻范围的最大 x 坐标。
     * @param jumpVel 跳跃速度向量 (水平, 垂直)。
     * @param jumpInterval 两次跳跃之间的间隔时间。
     */
    JumpBehavior(float minX, float maxX, glm::vec2 jumpVel = glm::vec2(100.0f, -300.0f), float jumpInterval = 2.0f);
    ~JumpBehavior() override = default;

    // 禁止拷贝和移动
    JumpBehavior(const JumpBehavior&) = delete;
    JumpBehavior& operator=(const JumpBehavior&) = delete;
    JumpBehavior(JumpBehavior&&) = delete;
    JumpBehavior& operator=(JumpBehavior&&) = delete;

private:
    void update(float deltaTime, AIComponent& AIComponent) override;
};

} // namespace game::component::ai