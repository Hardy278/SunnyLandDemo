#include "TextureManager.hpp"

#include <stdexcept>

#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>
#include <spdlog/spdlog.h>

namespace engine::resource {

void TextureManager::SDLTextureDeleter::operator()(SDL_Texture* texture) const {
    SDL_DestroyTexture(texture);
}

TextureManager::TextureManager(SDL_Renderer *renderer) : m_renderer(renderer) {
    if (!m_renderer) {
        throw std::runtime_error("RESOURCEMANAGER::TEXTUREMANAGER::SDL_Renderer未初始化, 请检查是否正确初始化SDL");
    }
    // 初始化SDL_Image
    spdlog::trace("RESOURCEMANAGER::TEXTUREMANAGER::TextureManager初始化成功");
}

TextureManager::~TextureManager() {
    spdlog::trace("RESOURCEMANAGER::TEXTUREMANAGER::TextureManager退出成功");
}

/// @name loader / unloader / getter
/// @{

SDL_Texture *TextureManager::loadTexture(const std::string_view path) {
    auto it = m_textures.find(std::string(path));
    if (it != m_textures.end()) {
        spdlog::warn("RESOURCEMANAGER::TEXTUREMANAGER::已存在同名纹理, 将使用原纹理");
        return it->second.get();
    }
    SDL_Texture *rawTexture = IMG_LoadTexture(m_renderer, std::string(path).c_str());
    // 载入纹理时，设置纹理缩放模式为最邻近插值(必不可少，否则TileLayer渲染中会出现边缘空隙/模糊)
    if (!SDL_SetTextureScaleMode(rawTexture, SDL_SCALEMODE_NEAREST)) {
        spdlog::warn("RESOURCEMANAGER::TEXTUREMANAGER::无法设置纹理缩放模式为最邻近插值");
    }
    if (!rawTexture) {
        spdlog::error("RESOURCEMANAGER::TEXTUREMANAGER::loadTexture::加载纹理失败: {} : {}", path, SDL_GetError());
        return nullptr;
    }
    m_textures.emplace(path, std::unique_ptr<SDL_Texture, SDLTextureDeleter>(rawTexture));
    spdlog::debug("RESOURCEMANAGER::TEXTUREMANAGER::loadTexture::加载纹理成功: {}", path);

    return rawTexture;
}

SDL_Texture *TextureManager::getTexture(const std::string_view path) {
    auto it = m_textures.find(std::string(path));
    if (it != m_textures.end()) {
        return it->second.get();
    }
    spdlog::warn("RESOURCEMANAGER::TEXTUREMANAGER::getTextureSize::未找到纹理, 尝试加载: {}", path);
    return loadTexture(path);
}

glm::vec2 TextureManager::getTextureSize(const std::string_view path) {
    SDL_Texture *texture = getTexture(path);
    if (!texture) {
        spdlog::error("RESOURCEMANAGER::TEXTUREMANAGER::getTextureSize::未找到纹理 \"{}\"", path);
        return glm::vec2(0, 0);
    }
    glm::vec2 size;
    if (!SDL_GetTextureSize(texture, &size.x, &size.y)) {
        spdlog::error("RESOURCEMANAGER::TEXTUREMANAGER::getTextureSize::获取纹理大小失败: {}", SDL_GetError());
        return glm::vec2(0, 0);
    }
    return size;
}

void TextureManager::unloadTexture(const std::string_view path) {
    auto it = m_textures.find(std::string(path));
    if (it != m_textures.end()) {
        m_textures.erase(it);
        spdlog::debug("RESOURCEMANAGER::TEXTUREMANAGER::unloadTexture::卸载纹理 \"{}\" 成功", path);
    } else {
        spdlog::warn("RESOURCEMANAGER::TEXTUREMANAGER::unloadTexture::未找到纹理 \"{}\"", path);
    }
}

void TextureManager::clearTextures() {
    if (!m_textures.empty()) {
        m_textures.clear();
        spdlog::debug("RESOURCEMANAGER::TEXTUREMANAGER::clearTextures::正在清理 {} 个缓存的纹理...", m_textures.size());
    } else {
        spdlog::warn("RESOURCEMANAGER::TEXTUREMANAGER::clearTextures::纹理列表为空, 无需清理");
    }
}
/// @}

} // namespace engine::resource