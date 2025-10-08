#pragma once
#include "Component.hpp"

#include <utility>
#include <glm/vec2.hpp>

namespace engine::component {
/**
 * @brief 变换组件，用于管理游戏对象的位置、旋转和缩放
 * 
 * TransformComponent是游戏对象的基础组件之一，负责处理对象的空间变换信息。
 * 它提供了2D空间中的位置、旋转和缩放功能，是游戏对象进行空间变换的核心组件。
 * 
 * @note 该组件为final类，不允许被继承
 * @note 旋转角度使用角度制（度）作为单位，而不是弧度
 * @note 缩放默认为(1.0f, 1.0f)，表示原始大小
 */
class TransformComponent final : public Component {
    friend class engine::object::GameObject;        // 友元不能继承，必须每个子类单独添加

public:
    glm::vec2 m_position = {0.0f, 0.0f};     ///< @brief 对象在2D空间中的位置坐标
    glm::vec2 m_scale = {1.0f, 1.0f};        ///< @brief 对象在X和Y轴上的缩放比例
    float m_rotation = 0.0f;                 ///< @brief 对象的旋转角度（角度制）

    /**
     * @brief 构造函数
     * @param position 初始位置，默认为原点(0,0)
     * @param scale 初始缩放，默认为不缩放(1,1)
     * @param rotation 初始旋转角度（角度制），默认为0度
     */
    TransformComponent(glm::vec2 position = {0.0f, 0.0f}, glm::vec2 scale = {1.0f, 1.0f}, float rotation = 0.0f)
        : m_position(std::move(position)), m_scale(std::move(scale)), m_rotation(rotation) {}

    /// @name 禁止拷贝和移动
    /// @{
    TransformComponent(const TransformComponent&) = delete;
    TransformComponent& operator=(const TransformComponent&) = delete;
    TransformComponent(TransformComponent&&) = delete;
    TransformComponent& operator=(TransformComponent&&) = delete;
    /// @}


    /// @name setter / getter
    /// @{
    /**
     * @brief 获取对象当前位置
     * @return const glm::vec2& 返回位置坐标的常量引用
     */
    const glm::vec2& getPosition() const;
    /**
     * @brief 获取对象当前旋转角度
     * @return float 返回旋转角度（角度制）
     */
    float getRotation() const;
    /**
     * @brief 获取对象当前缩放比例
     * @return const glm::vec2& 返回缩放比例的常量引用
     */
    const glm::vec2& getScale() const;
    /**
     * @brief 设置对象位置
     * @param position 新的位置坐标
     */
    void setPosition(glm::vec2 position);
    /**
     * @brief 设置对象旋转角度
     * @param rotation 新的旋转角度（角度制）
     */
    void setRotation(float rotation);
    /**
     * @brief 设置对象缩放比例
     * @param scale 新的缩放比例
     * @note 设置缩放时，如果对象有Sprite组件，应该同步更新Sprite的偏移量
     */
    void setScale(glm::vec2 scale);
    /// @}

    
    /**
     * @brief 平移对象
     * @param offset 平移的偏移量
     * @note 该方法会在当前位置的基础上加上偏移量
     */
    void translate(const glm::vec2& offset);

private:
    /**
     * @brief 更新组件状态
     * @param deltaTime 距离上一帧的时间（秒）
     * @param context 游戏上下文
     * @note TransformComponent不需要每帧更新，因此该函数为空实现
     */
    void update(float, engine::core::Context&) override {}
};
} // namespace engine::component