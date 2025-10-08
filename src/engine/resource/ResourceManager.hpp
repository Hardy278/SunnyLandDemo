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
    std::unique_ptr<FontManager> m_fontManager;
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


/// @name --- 统一资源访问接口 ---
/// @{

    /// @name --- Texture ---
    /// @{
    /**
     * @brief 从文件加载纹理并存储在纹理管理器中
     * @param path 纹理的路径
     * @return 加载的纹理指针
     */
    SDL_Texture* loadTexture(const std::string_view path);
    /**
     * @brief 从纹理管理器中获取纹理
     * @param path 纹理的路径
     * @return 纹理指针
     */
    SDL_Texture* getTexture(const std::string_view path);
    /**
     * @brief 获取纹理的大小
     * @param path 纹理的路径
     * @return 纹理的大小
     */
    glm::vec2 getTextureSize(const std::string_view path);
    /**
     * @brief 从纹理管理器中删除纹理
     * @param path 纹理的路径
     */
    void unloadTexture(const std::string_view path);
    
    /// @brief 清空纹理管理器中的所有纹理
    void clearTextures();
    /// @}

    
    /// @name --- Font ---
    /// @{
    /**
     * @brief 加载字体
     * @param path 字体文件路径
     * @param path 字体路径
     * @param size 字体大小
     * @return TTF_Font* 字体指针
     */
    TTF_Font* loadFont(const std::string_view path, int size);
    /**
     * @brief 获取字体
     * @param path 字体名称
     * @param size 字体路径
     * @return TTF_Font* 字体指针
     */
    TTF_Font* getFont(const std::string_view path, int size);
    /**
     * @brief 卸载字体
     * @param path 字体路径
     * @param size 字体大小
     */
    void unloadFont(const std::string_view path, int size);
    
    /// @brief 卸载所有字体
    void clearFonts();
    /// @}

/// @}
};

} // namespace engine::resource