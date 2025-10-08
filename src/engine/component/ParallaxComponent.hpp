#pragma once
#include "Component.hpp"
#include "../render/Sprite.hpp"
#include <string>
#include <string_view>
#include <glm/vec2.hpp>

namespace engine::component {
class TransformComponent;

class ParallaxComponent final : public Component {
    friend class engine::object::GameObject;
private:
    TransformComponent* m_transform = nullptr;   ///< @brief 缓存变换组件

    engine::render::Sprite m_sprite;             ///< @brief 精灵对象
    glm::vec2 m_scrollFactor;                   ///< @brief 滚动速度因子 (0=静止, 1=随相机移动, <1=比相机慢)
    glm::bvec2 m_repeat;                         ///< @brief 是否沿着X和Y轴周期性重复
    bool m_isHidden = false;                    ///< @brief 是否隐藏（不渲染）

public:
    /**
     * @brief 构造函数
     * @param textureId 背景纹理的资源 ID。
     * @param scrollFactor 控制背景相对于相机移动速度的因子。
     *                      (0, 0) 表示完全静止。
     *                      (1, 1) 表示与相机完全同步移动。
     *                      (0.5, 0.5) 表示以相机一半的速度移动。
     */
    ParallaxComponent(std::string_view textureID, glm::vec2 scrollFactor, glm::bvec2 repeat);

    // --- 设置器 ---
    void setSprite(const engine::render::Sprite& sprite) { m_sprite = sprite; }  ///< @brief 设置精灵对象
    void setScrollFactor(glm::vec2 factor) { m_scrollFactor = std::move(factor); }  ///< @brief 设置滚动速度因子
    void setRepeat(glm::bvec2 repeat) { m_repeat = std::move(repeat); }              ///< @brief 设置是否重复
    void setHidden(bool hidden) { m_isHidden = hidden; }                        ///< @brief 设置是否隐藏（不渲染）

    // --- 获取器 ---
    const engine::render::Sprite& getSprite() const { return m_sprite; }          ///< @brief 获取精灵对象
    const glm::vec2& getScrollFactor() const { return m_scrollFactor; }          ///< @brief 获取滚动速度因子
    const glm::bvec2& getRepeat() const { return m_repeat; }                      ///< @brief 获取是否重复
    bool isHidden() const { return m_isHidden; }                                  ///< @brief 获取是否隐藏（不渲染）

protected:
    // 核心循环函数覆盖
    void update(float, engine::core::Context&) override {}     // 必须实现纯虚函数，留空
    void init() override;                                      
    void render(engine::core::Context& context) override;      
};

} // namespace engine::component