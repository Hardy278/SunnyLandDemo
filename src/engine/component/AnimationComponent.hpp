#pragma once
#include "./Component.hpp"
#include <string>
#include <string_view>
#include <unordered_map>
#include <memory>

namespace engine::render {
    class Animation;
}
namespace engine::component {
    class SpriteComponent;
}

namespace engine::component {

/**
 * @brief GameObject的动画组件。
 *
 * 持有一组Animation对象并控制其播放，
 * 根据当前帧更新关联的SpriteComponent。
 */
class AnimationComponent : public Component {
    friend class engine::object::GameObject;
private:
    /// @brief 动画名称到Animation对象的映射。
    std::unordered_map<std::string, std::unique_ptr<engine::render::Animation>> m_animations;
    SpriteComponent* m_spriteComponent = nullptr;               ///< @brief 指向必需的SpriteComponent的指针
    engine::render::Animation* m_currentAnimation = nullptr;    ///< @brief 指向当前播放动画的原始指针

    float m_animationTimer = 0.0f;          ///< @brief 动画播放中的计时器
    bool m_isPlaying = false;               ///< @brief 当前是否有动画正在播放
    bool m_isOneShotRemoval = false;      ///< @brief 是否在动画结束后删除整个GameObject

public:
    AnimationComponent() = default;
    ~AnimationComponent() override;

    // 删除复制/移动操作
    AnimationComponent(const AnimationComponent&) = delete;
    AnimationComponent& operator=(const AnimationComponent&) = delete;
    AnimationComponent(AnimationComponent&&) = delete;
    AnimationComponent& operator=(AnimationComponent&&) = delete;

    void addAnimation(std::unique_ptr<engine::render::Animation> animation);    ///< @brief 向 m_animations map容器中添加一个动画。
    void playAnimation(std::string_view name);    ///< @brief 播放指定名称的动画。
    void stopAnimation() { m_isPlaying = false; }   ///< @brief 停止当前动画播放。
    void resumeAnimation() {m_isPlaying = true; }   ///< @brief 恢复当前动画播放。

    // --- Getters and Setters ---
    std::string_view getCurrentAnimationName() const;
    bool isPlaying() const { return m_isPlaying; }
    bool isAnimationFinished() const;
    bool isOneShotRemoval() const { return m_isOneShotRemoval; }
    void setOneShotRemoval(bool isOneShotRemoval) { m_isOneShotRemoval = isOneShotRemoval; }

protected:
    // 核心循环方法
    void init() override;
    void update(float, engine::core::Context&) override;
};

} // namespace engine::component