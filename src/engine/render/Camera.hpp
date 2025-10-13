/**
 * @file Camera.hpp
 * @brief 相机系统头文件
 */

#pragma once
#include "../utils/Math.hpp"
#include <optional>

namespace engine::component {
    class TransformComponent;
} // namespace engine::component

namespace engine::render {
/**
 * @class Camera
 * @brief 2D相机类，用于处理游戏世界的视图转换和相机移动
 * 
 * 该类实现了2D游戏中的相机系统，包括：
 * @brief - 世界坐标到屏幕坐标的转换
 * @brief - 视差滚动支持
 * @brief - 相机移动和边界限制
 * @brief - 视口管理
 */
class Camera final {
private:
    glm::vec2 m_viewportSize; ///< 视口大小
    glm::vec2 m_position; ///< 相机在世界坐标中的位置
    std::optional<engine::utils::Rect> m_limitBounds; ///< 相机移动的边界限制
    float m_smoothSpeed = 3.0f;                               ///< @brief 相机移动的平滑速度
    component::TransformComponent* m_target = nullptr;   ///< @brief 跟随目标变换组件，空值表示不跟随

public:
    /**
     * @brief 构造函数
     * @param viewportSize 视口大小
     * @param position 初始位置，默认为(0,0)
     * @param limitBounds 可选的相机移动边界限制
     */
    Camera(const glm::vec2& viewportSize, const glm::vec2& position = {0.0f, 0.0f}, const std::optional<engine::utils::Rect>& limitBounds = std::nullopt);


    /**
     * @brief 更新相机状态
     * @param deltaTime 上一帧到当前帧的时间间隔（秒）
     */
    void update(float deltaTime);
    /**
     * @brief 移动相机
     * @param offset 相机移动的偏移量
     */
    void move(const glm::vec2& offset);


    /// @name 转换方法
    /// @{
    /**
     * @brief 将世界坐标转换为屏幕坐标
     * @param worldPos 世界坐标位置
     * @return 转换后的屏幕坐标
     */
    glm::vec2 worldToScreen(const glm::vec2& worldPos) const;
    /**
     * @brief 带视差效果的世界坐标到屏幕坐标转换
     * @param worldPos 世界坐标位置
     * @param scrollFactor 视差滚动因子，(1,1)表示正常滚动，(0,0)表示完全不滚动
     * @return 转换后的屏幕坐标
     */
    glm::vec2 worldToScreenWithParallax(const glm::vec2& worldPos, const glm::vec2& scrollFactor) const;
    /**
     * @brief 将屏幕坐标转换为世界坐标
     * @param screenPos 屏幕坐标位置
     * @return 转换后的世界坐标
     */
    glm::vec2 screenToWorld(const glm::vec2& screenPos) const;
    /// @}


    /// @name getter / setter
    /// @{
    /**
     * @brief 设置相机位置
     * @param position 新的相机位置
     */
    void setPosition(const glm::vec2& position);
    /**
     * @brief 设置相机移动边界
     * @param linitBounds 相机移动的矩形边界
     */
    void setLimitBounds(std::optional<engine::utils::Rect> limitBounds);   ///< @brief 设置限制相机的移动范围
    ///< @brief 设置跟随目标变换组件
    void setTarget(engine::component::TransformComponent* target);

    /**
     * @brief 获取相机当前位置
     * @return 相机位置
     */
    const glm::vec2& getPosition() const;
    /**
     * @brief 获取相机移动边界
     * @return 相机移动边界，可能为空
     */
    const std::optional<engine::utils::Rect> getLimitBounds() const;
    /**
     * @brief 获取视口大小
     * @return 视口大小
     */
    const glm::vec2 getViewportSize() const;
    ///< @brief 获取跟随目标变换组件
    engine::component::TransformComponent* getTarget() const;
    /// @}

    /// @name 禁用拷贝构造和移动构造
    /// @{
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera(Camera&&) = delete;
    Camera& operator=(Camera&&) = delete;
    /// @}

private:
    /**
     * @brief 确保相机位置在边界范围内
     * 
     * 如果设置了边界限制，此方法会将相机位置限制在边界内。
     * 如果没有设置边界限制，此方法不执行任何操作。
     */
    void clampPosition();
};
} // namespace engine::render
