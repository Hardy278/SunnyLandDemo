#include "SpriteComponent.hpp"
#include "TransformComponent.hpp"
#include "../render/Sprite.hpp"
#include "../render/Renderer.hpp"
#include "../resource/ResourceManager.hpp"
#include "../object/GameObject.hpp"

#include <spdlog/spdlog.h>

namespace engine::component {

/// @name 构造函数
/// @{
SpriteComponent::SpriteComponent(
    std::string_view textureId, 
    engine::resource::ResourceManager &resourceManager, 
    engine::utils::Alignment alignment, 
    std::optional<SDL_FRect> sourceRectOpt, 
    bool isFlipped
) : m_sprite(textureId, std::move(sourceRectOpt), isFlipped), m_alignment(alignment), m_resourceManager(&resourceManager) {
    if (!m_resourceManager) {
        spdlog::critical("SPRITECOMPONENT::创建 SpriteComponent 时 ResourceManager 为空，此组件将无效");
        // 不要在游戏主循环中使用 try...catch / throw，会极大影响性能
    }
    // m_offset 和 m_spriteSize 将在 init 中计算
    spdlog::trace("SPRITECOMPONENT::创建 SpriteComponent, 纹理ID: {}", m_sprite.getTextureID());
}
SpriteComponent::SpriteComponent(
    engine::render::Sprite sprite, 
    engine::resource::ResourceManager &resourceManager, 
    engine::utils::Alignment alignment
) : m_sprite(std::move(sprite)), m_alignment(alignment), m_resourceManager(&resourceManager) {
    if (!m_resourceManager) {
        spdlog::critical("SPRITECOMPONENT::创建 SpriteComponent 时 ResourceManager 为空，此组件将无效");
        // 不要在游戏主循环中使用 try...catch / throw，会极大影响性能
    }
    // m_offset 和 m_spriteSize 将在 init 中计算
    spdlog::trace("SPRITECOMPONENT::创建 SpriteComponent, 纹理ID: {}", m_sprite.getTextureID());
}
/// @}


void SpriteComponent::updateOffset() {
    // 如果尺寸无效，偏移为0
    if (m_spriteSize.x <= 0 || m_spriteSize.y <= 0) {
        m_offset = {0.0f, 0.0f};
        return;
    }
    auto scale = m_transform->getScale();
    // 计算精灵左上角相对于 TransformComponent::position_ 的偏移
    switch (m_alignment) {
        case engine::utils::Alignment::TOP_LEFT:      m_offset = glm::vec2{0.0f, 0.0f} * scale; break;
        case engine::utils::Alignment::TOP_CENTER:    m_offset = glm::vec2{-m_spriteSize.x / 2.0f, 0.0f} * scale; break;
        case engine::utils::Alignment::TOP_RIGHT:     m_offset = glm::vec2{-m_spriteSize.x, 0.0f} * scale; break;
        case engine::utils::Alignment::CENTER_LEFT:   m_offset = glm::vec2{0.0f, -m_spriteSize.y / 2.0f} * scale; break;
        case engine::utils::Alignment::CENTER:        m_offset = glm::vec2{-m_spriteSize.x / 2.0f, -m_spriteSize.y / 2.0f} * scale; break;
        case engine::utils::Alignment::CENTER_RIGHT:  m_offset = glm::vec2{-m_spriteSize.x, -m_spriteSize.y / 2.0f} * scale; break;
        case engine::utils::Alignment::BOTTOM_LEFT:   m_offset = glm::vec2{0.0f, -m_spriteSize.y} * scale; break;
        case engine::utils::Alignment::BOTTOM_CENTER: m_offset = glm::vec2{-m_spriteSize.x / 2.0f, -m_spriteSize.y} * scale; break;
        case engine::utils::Alignment::BOTTOM_RIGHT:  m_offset = glm::vec2{-m_spriteSize.x, -m_spriteSize.y} * scale; break;
        case engine::utils::Alignment::NONE:
        default:                                      break;
    }
}

/// @name getter
/// @{
void SpriteComponent::setSpriteById(std::string_view textureID, std::optional<SDL_FRect> sourceRectOpt) {
    m_sprite.setTextureID(textureID);
    m_sprite.setSourceRect(std::move(sourceRectOpt));

    updateSpriteSize();
    updateOffset();
}
void SpriteComponent::setFlipped(bool flipped) {
    m_sprite.setFlipped(flipped);
}
void SpriteComponent::setHidden(bool hidden) {
    m_isHidden = hidden;
}
void SpriteComponent::setSourceRect(std::optional<SDL_FRect> sourceRectOpt) {
    m_sprite.setSourceRect(std::move(sourceRectOpt));
    updateSpriteSize();
    updateOffset();
}
void SpriteComponent::setAlignment(engine::utils::Alignment anchor) {
    m_alignment = anchor;
    updateOffset();
}
/// @}


void SpriteComponent::updateSpriteSize() {
    if (!m_resourceManager) {
        spdlog::error("SPRITECOMPONENT::updateSpriteSize::ResourceManager 为空！无法获取纹理尺寸");
        return;
    }
    if (m_sprite.getSourceRect().has_value()) {
        const auto& srcRect = m_sprite.getSourceRect().value();
        m_spriteSize = {srcRect.w, srcRect.h};
    } else {
        m_spriteSize = m_resourceManager->getTextureSize(m_sprite.getTextureID());
    }
}



/// @name Component 虚函数覆盖
/// @{
void SpriteComponent::init() {
    if (!m_owner) {
        spdlog::error("SPRITECOMPONENT::init::SpriteComponent 在初始化前未设置所有者。");
        return;
    }
    m_transform = m_owner->getComponent<TransformComponent>();
    if (!m_transform) {
        spdlog::warn("SPRITECOMPONENT::init::GameObject '{}' 上的 SpriteComponent 需要一个 TransformComponent, 但未找到", m_owner->getName());
        // Sprite没有Transform无法计算偏移和渲染，直接返回
        return;
    }
    // 获取大小及偏移
    updateSpriteSize();
    updateOffset();
}
void SpriteComponent::render(engine::core::Context &context) {
    if (m_isHidden || !m_transform || !m_resourceManager) {
        spdlog::error("SPRITECOMPONENT::render::SpriteComponent 在渲染前未设置 owner 或 TransformComponent 或 ResourceManager 为空");
        return;
    }

    // 获取变换信息（考虑偏移量）
    const glm::vec2& pos = m_transform->getPosition() + m_offset;
    const glm::vec2& scale = m_transform->getScale();
    float rotationDegrees = m_transform->getRotation();

    // 执行绘制
    context.getRenderer().drawSprite(context.getCamera(), m_sprite, pos, scale, rotationDegrees);
}
}