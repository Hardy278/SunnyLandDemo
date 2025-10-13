#pragma once
#include "Sprite.hpp"
#include "../utils/Math.hpp"

#include <glm/glm.hpp>

#include <optional>
#include <string>

struct SDL_Renderer;
struct SDL_FRect;

namespace engine::resource {
    class ResourceManager;
} // namespace engine::resource

namespace engine::render {
class Sprite;
class Camera;

/**
 * @class Renderer
 * @brief 渲染器类，负责处理所有与渲染相关的操作
 * 
 * 这个类封装了SDL渲染器的功能，提供了精灵绘制、视差滚动、UI元素渲染等功能。
 * 它与游戏资源管理器和相机系统紧密集成，以实现高效的渲染流程。
 */
class Renderer final {
private:
    SDL_Renderer* m_renderer = nullptr;           ///< SDL渲染器指针
    resource::ResourceManager* m_resourceManager = nullptr; ///< 资源管理器指针

public:
    /**
     * @brief 构造函数
     * @param renderer SDL渲染器指针，不能为nullptr
     * @param resourceManager 资源管理器指针，不能为nullptr
     * 
     * 初始化渲染器实例，关联SDL渲染器和资源管理器
     */
    Renderer(SDL_Renderer* renderer, resource::ResourceManager* resourceManager);


    /// @name 绘制部分
    /// @{
    /**
     * @brief 绘制游戏中的精灵
     * @param camera   相机对象，用于计算视口变换
     * @param sprite   要绘制的精灵对象
     * @param position 精灵在游戏世界中的位置
     * @param scale    精灵的缩放比例，默认为(1.0f, 1.0f)
     * @param angle    精灵的旋转角度（度），默认为0.0
     * 
     * 此方法会将精灵根据相机位置进行视口变换后绘制到屏幕上
     */
    void drawSprite(const Camera& camera, const Sprite& sprite, const glm::vec2& position,
        const glm::vec2& scale = glm::vec2(1.0f), double angle = 0.0);
        
    /**
     * @brief 绘制视差滚动背景
     * @param camera       相机对象，用于计算视口变换
     * @param sprite       要绘制的精灵对象（通常作为背景）
     * @param position     精灵在游戏世界中的位置
     * @param scrollFactor 滚动因子，控制背景移动速度（相机移动时背景移动的比例）
     * @param repeat       是否重复绘制精灵以覆盖整个视口，默认为两个方向都重复
     * @param scale        精灵的缩放比例，默认为(1.0f, 1.0f)
     * 
     * 此方法用于创建视差滚动效果，背景元素会以不同速度移动，增强深度感
     */
    void drawParallax(const Camera& camera, const Sprite& sprite, const glm::vec2& position,
        const glm::vec2& scrollFactor, const glm::bvec2& repeat = glm::bvec2(true), const glm::vec2& scale = glm::vec2(1.0f));
        
    /**
     * @brief 绘制UI精灵（不受相机影响）
     * @param sprite 要绘制的精灵对象
     * @param position 精灵在屏幕上的位置
     * @param size 精灵的尺寸（可选），如果不提供则使用精灵原始尺寸
     * 
     * 此方法用于绘制UI元素，它们不受相机变换影响，直接显示在屏幕上
     */
    void drawUISprite(const Sprite& sprite, const glm::vec2& position, const std::optional<glm::vec2>& size = std::nullopt);
    
    /**
     * @brief 绘制填充矩形
     * 
     * @param rect 矩形区域
     * @param color 填充颜色
     */
    void drawUIFilledRect(const engine::utils::Rect& rect, const engine::utils::FColor& color);
    /// @}



    /// @name 渲染部分
    /// @{
    /**
     * @brief 呈现当前渲染缓冲区到屏幕
     * 
     * 此方法将所有绘制操作的结果显示到屏幕上
     */
    void present();
    /**
     * @brief 清空屏幕
     * 
     * 使用当前设置的颜色清空屏幕，为下一帧渲染做准备
     */
    void clearScreen();
    /// @}


    /// @name getter / setter
    /// @{
    /**
     * @brief 设置绘制颜色（整数RGBA格式）
     * @param r 红色分量 (0-255)
     * @param g 绿色分量 (0-255)
     * @param b 蓝色分量 (0-255)
     * @param a alpha分量 (0-255)，默认为255（完全不透明）
     * 
     * 设置后续绘制操作使用的颜色
     */
    void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
    /**
     * @brief 设置绘制颜色（浮点数RGBA格式）
     * @param r 红色分量 (0.0-1.0)
     * @param g 绿色分量 (0.0-1.0)
     * @param b 蓝色分量 (0.0-1.0)
     * @param a alpha分量 (0.0-1.0)，默认为1.0（完全不透明）
     * 
     * 设置后续绘制操作使用的颜色，使用浮点数表示
     */
    void setDrawColorFloat(float r, float g, float b, float a = 1.0f);
    /**
     * @brief 获取底层SDL渲染器指针
     * @return SDL_Renderer* SDL渲染器指针
     * 
     * 返回与此渲染器关联的SDL渲染器指针，可用于需要直接操作SDL渲染器的特殊情况
     */
    SDL_Renderer* getSDLRenderer() const;
    /// @}

    
    /// @name 禁用拷贝移动构造
    /// @{
    Renderer(const Renderer&) = delete;  ///< 禁用拷贝构造
    Renderer& operator=(const Renderer&) = delete;  ///< 禁用拷贝赋值
    Renderer(Renderer&&) = delete;  ///< 禁用移动构造
    Renderer& operator=(Renderer&&) = delete;  ///< 禁用移动赋值
    /// @}
    
private:
    /**
     * @brief 获取精灵的源矩形
     * @param sprite 精灵对象
     * @return std::optional<SDL_FRect> 精灵的源矩形，如果没有有效矩形则返回空
     * 
     * 根据精灵的当前帧和动画状态，返回相应的源矩形区域
     */
    std::optional<SDL_FRect> getSpriteSrcRect(const Sprite& sprite);
    
    /**
     * @brief 检查矩形是否在视口中可见
     * @param camera 相机对象
     * @param rect 要检查的矩形
     * @return bool 如果矩形在视口中可见则返回true，否则返回false
     * 
     * 用于优化渲染，只绘制可见区域的精灵
     */
    bool isRectInViewport(const Camera& camera, const SDL_FRect& rect);
};

} // namespace engine::render