/**
 * @file SpriteComponent.hpp
 * @brief 精灵组件类，用于管理游戏对象的精灵渲染
 */

#pragma once
#include "Component.hpp"
#include "../render/Sprite.hpp"
#include "../utils/Alignment.hpp"

#include <SDL3/SDL_rect.h>
#include <glm/vec2.hpp>

#include <string>
#include <string_view>
#include <optional>

namespace engine::core {
    class Context;
} // namespace engine::core

namespace engine::resource {
    class ResourceManager;
} // namespace engine::resource

namespace engine::component {
class TransformComponent;
    
/**
 * @class SpriteComponent
 * @brief 精灵组件，用于管理游戏对象的精灵渲染
 * 
 * SpriteComponent是游戏引擎中用于渲染精灵图像的组件。它继承自Component基类，
 * 提供了设置精灵纹理、调整精灵属性、控制渲染行为等功能。该组件支持多种
 * 构造方式，可以基于纹理ID创建精灵，也可以直接传入Sprite对象。
 * 
 * 组件支持精灵的翻转、隐藏、对齐方式调整等功能，并能自动计算精灵尺寸。
 * 同时，它还与TransformComponent协作，实现精灵的位置和变换。
 */
class SpriteComponent final : public Component {
    friend class engine::object::GameObject;  ///< 声明为友元类，允许GameObject访问私有成员
private:
    resource::ResourceManager* m_resourceManager = nullptr;   ///< @brief 保存资源管理器指针，用于获取纹理大小
    TransformComponent*        m_transform       = nullptr;   ///< @brief 缓存 TransformComponent 指针（非必须）

    utils::Alignment m_alignment  = utils::Alignment::NONE;   ///< @brief 对齐方式
    render::Sprite   m_sprite;                                ///< @brief 精灵对象
    glm::vec2        m_spriteSize = {0.0f, 0.0f};             ///< @brief 精灵尺寸
    glm::vec2        m_offset     = {0.0f, 0.0f};             ///< @brief 偏移量
    bool             m_isHidden   = false;                    ///< @brief 是否隐藏（不渲染）

public:
    /// @name 构造函数 / 析构函数
    /// @{
    /**
     * @brief 构造函数，通过纹理ID创建精灵组件
     * @param textureId 纹理资源ID
     * @param resourceManager 资源管理器引用
     * @param alignment 精灵对齐方式，默认为NONE
     * @param sourceRectOpt 可选的源矩形，用于精灵纹理的裁剪，默认为整个纹理
     * @param isFlipped 是否水平翻转精灵，默认为false
     */
    SpriteComponent(
        std::string_view textureId,
        engine::resource::ResourceManager& resourceManager,
        engine::utils::Alignment alignment = engine::utils::Alignment::NONE,
        std::optional<SDL_FRect> sourceRectOpt = std::nullopt,
        bool isFlipped = false
    );
    /**
     * @brief 构造函数，通过Sprite对象创建精灵组件
     * @param sprite 精灵对象
     * @param resourceManager 资源管理器引用
     * @param alignment 精灵对齐方式，默认为NONE
     */
    SpriteComponent(
        engine::render::Sprite&& sprite,
        engine::resource::ResourceManager& resourceManager,
        engine::utils::Alignment alignment = engine::utils::Alignment::NONE
    );
    ~SpriteComponent() override = default;
    /// @}


    /**
     * @brief 更新精灵偏移量
     * 
     * 根据精灵的对齐方式和尺寸，计算并更新精灵的偏移量。
     */
    void updateOffset();


    /// @name 禁止拷贝和移动
    /// @{
    SpriteComponent(const SpriteComponent&) = delete;
    SpriteComponent& operator=(const SpriteComponent&) = delete;
    SpriteComponent(SpriteComponent&&) = delete;
    SpriteComponent& operator=(SpriteComponent&&) = delete;
    /// @}


    /// @name setter
    /// @{
    const render::Sprite &getSprite() const { return m_sprite; }
    const glm::vec2 &getSpriteSize() const { return m_spriteSize; }
    const glm::vec2 &getOffset() const { return m_offset; }
    utils::Alignment getAlignment() { return m_alignment; }
    std::string_view getTextureID() const { return m_sprite.getTextureID(); }
    bool isFlipped() const { return m_sprite.isFlipped(); }
    bool isHidden() const { return m_isHidden; }
    /// @}


    /// @name getter
    /// @{
    /**
     * @brief 通过纹理ID设置精灵
     * @param textureID 纹理资源ID
     * @param sourceRectOpt 可选的源矩形，用于精灵纹理的裁剪，默认为整个纹理
     */
    void setSpriteById(std::string_view textureID, std::optional<SDL_FRect> sourceRectOpt = std::nullopt); 
    /**
     * @brief 设置精灵是否翻转
     * @param flipped 是否水平翻转精灵
     */
    void setFlipped(bool flipped);
    /**
     * @brief 设置精灵是否隐藏
     * @param hidden 是否隐藏精灵
     */
    void setHidden(bool hidden);
    /**
     * @brief 设置精灵的源矩形
     * @param sourceRectOpt 可选的源矩形，用于精灵纹理的裁剪，设为std::nullopt则使用整个纹理
     */
    void setSourceRect(std::optional<SDL_FRect> sourceRectOpt);
    /**
     * @brief 设置精灵的对齐方式
     * @param anchor 新的对齐方式
     */
    void setAlignment(engine::utils::Alignment anchor);
    /// @}

private:
    /**
     * @brief 更新精灵尺寸
     * 
     * 根据精灵的源矩形更新精灵尺寸。如果源矩形未设置，则使用整个纹理的尺寸。
     */
    void updateSpriteSize();

    /// @name Component 虚函数覆盖
    /// @{
    /**
     * @brief 初始化组件
     * 
     * 重写基类的初始化函数，用于初始化精灵组件的必要资源。
     */
    void init() override;
    /**
     * @brief 更新组件
     * 
     * 重写基类的更新函数，当前实现为空，因为精灵的渲染主要由渲染系统处理。
     * @param deltaTime 自上一帧以来经过的时间（秒）
     * @param context 引擎上下文
     */
    void update(float, engine::core::Context&) override {}
    /**
     * @brief 渲染组件
     * 
     * 重写基类的渲染函数，负责将精灵渲染到屏幕上。
     * @param context 引擎上下文
     */
    void render(engine::core::Context& context) override;
    /// @}
};
} // namespace engine::component