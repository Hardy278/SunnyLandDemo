#pragma once
#include <unordered_map>
#include <string>
#include <string_view>
#include <memory>

#include <glm/glm.hpp>

struct SDL_Texture;
struct SDL_Renderer;

namespace engine::resource {

/**
 * @class TextureManager
 * @brief 纹理管理器，用于加载和管理纹理资源
 * @note 纹理管理器是单例模式，通过 ResourceManager 获取，不可直接访问
 * @note 纹理管理器使用智能指针管理纹理的生命周期
 */
class TextureManager final {
    friend class ResourceManager; // 友元类，允许 ResourceManager 访问私有成员
private:
    /** 
     * @struct SDLTextureDeleter
     * @brief SDL 纹理的删除器，用于在纹理管理器中删除纹理
     * @note 纹理管理器使用智能指针管理纹理的生命周期，需要自定义删除器
     */
    struct SDLTextureDeleter {
        void operator()(SDL_Texture* texture) const; // 定义删除器函数
    };
    std::unordered_map<std::string, std::unique_ptr<SDL_Texture, SDLTextureDeleter>> m_textures = {};
    SDL_Renderer* m_renderer = nullptr;

public:
    /**
     * @brief 构造函数，初始化 TextureManager
     * @param renderer 渲染器指针
     * @throw std::runtime_error 如果渲染器为空或初始化失败，则抛出异常
     */
    explicit TextureManager(SDL_Renderer* renderer);
    ~TextureManager(); // 析构函数，清空纹理管理器中的所有纹理

    /// @name 删除移动拷贝构造函数
    /// @{
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    TextureManager(TextureManager&&) = delete;
    TextureManager& operator=(TextureManager&&) = delete;
    /// @}

private:
    /// @name loader / unloader / getter
    /// @{
    /**
     * @brief 从文件加载纹理并存储在纹理管理器中
     * @param path 纹理文件的路径
     * @param name 纹理的名称
     * @return 加载的纹理指针
     */
    SDL_Texture* loadTexture(const std::string_view path);
    /**
     * @brief 从纹理管理器中获取纹理
     * @param name 纹理的名称
     * @return 纹理指针
     */
    SDL_Texture* getTexture(const std::string_view path);
    /**
     * @brief 获取纹理的大小
     * @param name 纹理的名称
     * @return 纹理的大小
     */
    glm::vec2 getTextureSize(const std::string_view path);
    /**
     * @brief 从纹理管理器中删除纹理
     * @param name 纹理的名称
     */
    void unloadTexture(const std::string_view path);
    
    /// @brief 清空纹理管理器中的所有纹理
    void clearTextures();
    /// @}

};

}