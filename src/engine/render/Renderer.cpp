#include "Renderer.hpp"
#include "Camera.hpp"
#include "../resource/ResourceManager.hpp"

#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include <stdexcept>

namespace engine::render {

Renderer::Renderer(SDL_Renderer *renderer, resource::ResourceManager *resourceManager)
    : m_renderer(renderer), m_resourceManager(resourceManager) {
    spdlog::trace("RENDERER::正在初始化...");
    if (!m_renderer) {
        throw std::runtime_error("RENDERER::ERROR::初始化失败: SDL_Renderer为空指针");
    }
    if (!m_resourceManager) {
        throw std::runtime_error("RENDERER::ERROR::初始化失败: ResourceManager为空指针");
    }
    setDrawColor(0, 0, 0, 255); // 设置默认颜色为黑色
    spdlog::trace("RENDERER::初始化成功");
}


/// @name 绘制部分
/// @{
void Renderer::drawSprite(const Camera &camera, const Sprite &sprite, const glm::vec2 &position, const glm::vec2 &scale, double angle) {
    auto texture = m_resourceManager->getTexture(sprite.getTextureID());
    if (!texture) {
        spdlog::error("RENDERER::drawSprite::ERROR::获取纹理失败: 纹理ID为{}", sprite.getTextureID());
        return;
    }

    auto srcRect = getSpriteSrcRect(sprite);
    if (!srcRect.has_value()) {
        spdlog::error("RENDERER::drawSprite::ERROR::获取精灵原矩形失败: 纹理ID为{}", sprite.getTextureID());
        return;
    }

    glm::vec2 positionScreen = camera.worldToScreen(position);

    float scaledWidth = srcRect.value().w * scale.x;
    float scaledHeight = srcRect.value().h * scale.y;

    SDL_FRect destRect = {positionScreen.x, positionScreen.y, scaledWidth, scaledHeight};
    // 视口裁剪
    if (!isRectInViewport(camera, destRect)) return;

    if (!SDL_RenderTextureRotated(m_renderer, texture, &srcRect.value(), &destRect, angle, NULL, sprite.isFlipped() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)) {
        spdlog::error("RENDERER::drawSprite::ERROR::渲染精灵失败: 纹理ID为{} : {}", sprite.getTextureID(), SDL_GetError());
    }
}

void Renderer::drawParallax(const Camera &camera, const Sprite &sprite, const glm::vec2 &position, const glm::vec2 &scrollFactor, const glm::bvec2 &repeat, const glm::vec2 &scale) {
    auto texture = m_resourceManager->getTexture(sprite.getTextureID());
    if (!texture) {
        spdlog::error("RENDERER::drawParallax::ERROR::获取纹理失败: 纹理ID为{}", sprite.getTextureID());
        return;
    }
    auto srcRect = getSpriteSrcRect(sprite);
    if (!srcRect.has_value()) {
        spdlog::error("RENDERER::drawParallax::ERROR::获取精灵原矩形失败: 纹理ID为{}", sprite.getTextureID());
        return;
    }
    glm::vec2 positionScreen = camera.worldToScreenWithParallax(position, scrollFactor);

    float scaledTextureWidth = srcRect.value().w * scale.x;
    float scaledTextureHeight = srcRect.value().h * scale.y;

    glm::vec2 start, stop;
    glm::vec2 viewportSize = camera.getViewportSize();

    if (repeat.x) {
        start.x = glm::mod(positionScreen.x, scaledTextureWidth) - scaledTextureWidth;
        stop.x = viewportSize.x;
    } else {
        start.x = positionScreen.x;
        stop.x = glm::min(positionScreen.x + scaledTextureWidth, viewportSize.x);
    }

    if (repeat.y) {
        start.y = glm::mod(positionScreen.y, scaledTextureHeight) - scaledTextureHeight;
        stop.y = viewportSize.y;
    } else {
        start.y = positionScreen.y;
        stop.y = glm::min(positionScreen.y + scaledTextureHeight, viewportSize.y);
    }

    for (float y = start.y; y < stop.y; y += scaledTextureHeight) {
        for (float x = start.x; x < stop.x; x += scaledTextureWidth) {
            SDL_FRect dstRect = {x, y, scaledTextureWidth, scaledTextureHeight};
            if (!SDL_RenderTexture(m_renderer, texture, nullptr, &dstRect)) {
                spdlog::error("RENDERER::drawParallax::ERROR::渲染精灵失败: 纹理ID为{} : {}", sprite.getTextureID(), SDL_GetError());
                return;
            }
        }
    }
}

void Renderer::drawUISprite(const Sprite &sprite, const glm::vec2 &position, const std::optional<glm::vec2> &size) {
    auto texture = m_resourceManager->getTexture(sprite.getTextureID());
    if (!texture) {
        spdlog::error("RENDERER::drawUISprite::ERROR::获取纹理失败: 纹理ID为{}", sprite.getTextureID());
        return;
    }
    auto srcRect = getSpriteSrcRect(sprite);
    if (!srcRect.has_value()) {
        spdlog::error("RENDERER::drawUISprite::ERROR::获取精灵原矩形失败: 纹理ID为{}", sprite.getTextureID());
        return;
    }

    SDL_FRect destRect = {position.x, position.y, 0, 0}; // 初始化 destRect, 后续会根据 size 和 srcRect 进行设置
    if (size.has_value()) {              // 如果提供尺寸，则使用提供的尺寸
        destRect.w = size.value().x;
        destRect.h = size.value().y;
    } else {                  // 如果没有提供尺寸，则使用纹理的原始尺寸
        destRect.w = srcRect.value().w;
        destRect.h = srcRect.value().h;
    }

    if (!SDL_RenderTextureRotated(m_renderer, texture, &srcRect.value(), &destRect, 0.0, NULL, sprite.isFlipped() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)) {
        spdlog::error("RENDERER::drawUISprite::ERROR::渲染 UI Sprite 失败: 纹理ID为{} : {}", sprite.getTextureID(), SDL_GetError());
    }
}
void Renderer::drawUIFilledRect(const engine::utils::Rect &rect, const engine::utils::FColor &color) {
    setDrawColorFloat(color.r, color.g, color.b, color.a);
    SDL_FRect SDLRect = {rect.position.x, rect.position.y, rect.size.x, rect.size.y};
    if (!SDL_RenderFillRect(m_renderer, &SDLRect)) {
        spdlog::error("RENDERER::drawUIFilledRect::ERROR::绘制填充矩形失败：{}", SDL_GetError());
    }
    setDrawColor(0, 0, 0, 1.0f);
}
/// @}

/// @name 渲染部分
/// @{
void Renderer::present() {
    SDL_RenderPresent(m_renderer);
}

void Renderer::clearScreen() {
    if (!SDL_RenderClear(m_renderer)) {
        spdlog::error("RENDERER::clearScreen::ERROR::清屏失败: {}", SDL_GetError());
    }
}
/// @}

/// @name getter / setter
/// @{
void Renderer::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    if (!SDL_SetRenderDrawColor(m_renderer, r, g, b, a)) {
        spdlog::error("RENDERER::setDrawColor::ERROR::设置绘制颜色失败: {}", SDL_GetError());
    }
}

void Renderer::setDrawColorFloat(float r, float g, float b, float a) {
    if (!SDL_SetRenderDrawColorFloat(m_renderer, r, g, b, a)) {
        spdlog::error("RENDERER::setDrawColorFloat::ERROR::设置绘制颜色失败: {}", SDL_GetError());
    }
}

SDL_Renderer *Renderer::getSDLRenderer() const {
    return m_renderer;
}
/// @}

std::optional<SDL_FRect> Renderer::getSpriteSrcRect(const Sprite &sprite) {
    SDL_Texture *texture = m_resourceManager->getTexture(sprite.getTextureID());
    if (!texture) {
        spdlog::error("RENDERER::getSpriteSrcRect::ERROR::获取纹理失败: 纹理ID为{}", sprite.getTextureID());
        return std::nullopt;
    }

    auto srcRect = sprite.getSourceRect();
    if (srcRect.has_value()) {
        if (srcRect.value().w <= 0 || srcRect.value().h <= 0) {
            spdlog::error("RENDERER::getSpriteSrcRect::ERROR::精灵原矩形错误: 纹理ID为{}", sprite.getTextureID());
            return std::nullopt;
        }
        return srcRect;
    } else {
        SDL_FRect result = {0, 0, 0, 0};
        if(!SDL_GetTextureSize(texture, &result.w, &result.h)) {
            spdlog::error("RENDERER::getSpriteSrcRect::ERROR::获取精灵原矩形失败: 纹理ID为{}", sprite.getTextureID());
            return std::nullopt;
        }
        return result;
    }
}

bool Renderer::isRectInViewport(const Camera &camera, const SDL_FRect &rect) {
    glm::vec2 viewportSize = camera.getViewportSize();
    return rect.x + rect.w >= 0 && rect.x <= viewportSize.x &&  // 相当于 AABB 的碰撞检测
           rect.y + rect.h >= 0 && rect.y <= viewportSize.y;
}

} // namespace engine::render