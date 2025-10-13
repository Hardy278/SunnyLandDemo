#pragma once
#include "UIElement.hpp"
#include "../render/Sprite.hpp"

#include <SDL3/SDL_rect.h>

#include <string>
#include <string_view>
#include <optional>

namespace engine::ui {

/**
 * @brief 一个用于显示纹理或部分纹理的UI元素。
 *
 * 继承自UIElement并添加了渲染图像的功能。
 */
class UIImage final : public UIElement {
protected:
    engine::render::Sprite m_sprite;

public:
    /**
     * @brief 构造一个UIImage对象。
     *
     * @param textureID 要显示的纹理ID。
     * @param position 图像的局部位置。
     * @param size 图像元素的大小。（如果为{0,0}，则使用纹理的原始尺寸）
     * @param sourceRect 可选：要绘制的纹理部分。（如果为空，则使用纹理的整个区域）
     * @param isFlipped 可选：精灵是否应该水平翻转。
     */
    UIImage(
        std::string_view textureID,
        glm::vec2 position = {0.0f, 0.0f},
        glm::vec2 size = {0.0f, 0.0f},
        std::optional<SDL_FRect> sourceRect = std::nullopt,
        bool isFlipped = false
    );

    // --- 核心方法 ---
    void render(engine::core::Context& context) override;

    // --- Setters & Getters ---
    const engine::render::Sprite& getSprite() const { return m_sprite; }
    void setSprite(engine::render::Sprite sprite) { m_sprite = std::move(sprite); }

    std::string_view getTextureID() const { return m_sprite.getTextureID(); }
    void setTextureID(std::string_view textureID) { m_sprite.setTextureID(textureID); }

    const std::optional<SDL_FRect>& getSourceRect() const { return m_sprite.getSourceRect(); }
    void setSourceRect(std::optional<SDL_FRect> sourceRect) { m_sprite.setSourceRect(std::move(sourceRect)); }

    bool isFlipped() const { return m_sprite.isFlipped(); }
    void setFlipped(bool flipped) { m_sprite.setFlipped(flipped); }
};

} // namespace engine::ui