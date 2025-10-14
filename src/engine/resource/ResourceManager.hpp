#pragma once
#include <memory>
#include <string>
#include <string_view>

#include <glm/glm.hpp>

// SDL 前向声明
struct SDL_Renderer;
struct SDL_Texture;
struct Mix_Chunk;
struct Mix_Music;
struct TTF_Font;

namespace engine::resource {
// 资源管理器 前向声明
class TextureManager;
class FontManager;

/**
 * @class ResourceManager
 * @brief 资源管理器
 * @note 资源管理器负责管理所有的资源，包括纹理、字体等
 */
class ResourceManager final {
private:
    /// @name 资源管理器子组件对象
    /// @{
    std::unique_ptr<TextureManager> m_textureManager;
    std::unique_ptr<FontManager>    m_fontManager;
    /// @}
public:
    /**
     * @brief 资源管理器构造函数
     * @param renderer SDL 渲染器，传递给 TextureManager，不能为空
     * @note 用了 explicit 关键字，禁止隐式转换
     */
    explicit ResourceManager(SDL_Renderer *renderer);
    ~ResourceManager();

    /// @brief 清理所有资源
    void clear();

    /// @name 除移动拷贝构造函数
    /// @{
    ResourceManager(const ResourceManager &) = delete;
    ResourceManager &operator=(const ResourceManager &) = delete;
    ResourceManager(ResourceManager &&) = delete;
    ResourceManager &operator=(ResourceManager &&) = delete;
    /// @}

    /// @name --- Texture ---
    /// @{
    SDL_Texture* loadTexture(const std::string_view path);
    SDL_Texture* getTexture(const std::string_view path);
    glm::vec2 getTextureSize(const std::string_view path);
    void unloadTexture(const std::string_view path);
    void clearTextures();
    /// @}

    
    /// @name --- Font ---
    /// @{
    TTF_Font* loadFont(const std::string_view path, int size);
    TTF_Font* getFont(const std::string_view path, int size);
    void unloadFont(const std::string_view path, int size);
    void clearFonts();
    /// @}

/// @}
};

} // namespace engine::resource