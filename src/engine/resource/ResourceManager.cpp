#include "ResourceManager.hpp"
#include "TextureManager.hpp"
#include "FontManager.hpp"

#include <spdlog/spdlog.h>

namespace engine::resource {
ResourceManager::ResourceManager(SDL_Renderer *renderer) {
    spdlog::trace("RESOURCESMANAGER::初始化中...");
    m_textureManager = std::make_unique<TextureManager>(renderer);
    m_fontManager = std::make_unique<FontManager>();

    spdlog::trace("RESOURCESMANAGER::初始化成功");
}

ResourceManager::~ResourceManager() = default;

void ResourceManager::clear() {
    m_textureManager->clearTextures();
    m_fontManager->clearFonts();

    spdlog::trace("RESOURCESMANAGER::资源清理成功");
}

/// @name --- Texture ---
/// @{
SDL_Texture* ResourceManager::loadTexture(const std::string_view path) { return m_textureManager->loadTexture(path); }
SDL_Texture* ResourceManager::getTexture(const std::string_view path) { return m_textureManager->getTexture(path); }
glm::vec2 ResourceManager::getTextureSize(const std::string_view path) { return m_textureManager->getTextureSize(path); }
void ResourceManager::unloadTexture(const std::string_view path) { m_textureManager->unloadTexture(path); }
void ResourceManager::clearTextures() { m_textureManager->clearTextures(); }
/// @}

/// @name --- Font ---
/// @{
TTF_Font* ResourceManager::loadFont(const std::string_view path, int size) { return m_fontManager->loadFont(path, size); }
TTF_Font* ResourceManager::getFont(const std::string_view path, int size) { return m_fontManager->getFont(path, size); }
void ResourceManager::unloadFont(const std::string_view path, int size) { m_fontManager->unloadFont(path, size); }
void ResourceManager::clearFonts() { m_fontManager->clearFonts(); }
/// @}

} // namespace engine::resource