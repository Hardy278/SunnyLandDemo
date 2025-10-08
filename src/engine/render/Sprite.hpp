/**
 * @file Sprite.h
 * @brief 精灵类头文件
 */

#pragma once
#include <SDL3/SDL_rect.h>

#include <optional>
#include <string>
#include <string_view>

namespace engine::render {

/**
 * @class Sprite
 * @brief 精灵类，用于管理2D图形渲染的基本单元
 * 
 * Sprite类封装了纹理ID、源矩形区域和翻转状态等属性，
 * 是2D游戏引擎中用于渲染精灵图的基础类。
 */
class Sprite final {
private:
    std::string m_textureID = "";         ///< 纹理ID标识符
    std::optional<SDL_FRect> m_sourceRect; ///< 源矩形区域，可选
    bool m_isFlipped = false;             ///< 是否水平翻转

public:
    /**
     * @brief 默认构造函数（创建一个空的/无效的精灵）
     */
    Sprite() = default;
    
    /**
     * @brief 构造函数
     * @param textureID 纹理ID标识符
     * @param sourceRect 源矩形区域，可选参数
     * @param isFlipped 是否水平翻转，默认为false
     */
    Sprite(const std::string_view textureID, const std::optional<SDL_FRect>& sourceRect = std::nullopt, bool isFlipped = false);

    /// @name getter / setter
    /// @{
    
    /**
     * @brief 获取纹理ID
     * @return const std::string& 纹理ID的常量引用
     */
    const std::string_view getTextureID() const;
    
    /**
     * @brief 获取源矩形区域
     * @return const std::optional<SDL_Rect>& 源矩形区域的常量引用
     */
    const std::optional<SDL_FRect>& getSourceRect() const;
    
    /**
     * @brief 获取翻转状态
     * @return bool 是否被翻转
     */
    bool isFlipped() const;

    /**
     * @brief 设置纹理ID
     * @param textureID 新的纹理ID
     */
    void setTextureID(const std::string_view textureID);
    
    /**
     * @brief 设置源矩形区域
     * @param sourceRect 新的源矩形区域，可选
     */
    void setSourceRect(const std::optional<SDL_FRect>& sourceRect);
    
    /**
     * @brief 设置翻转状态
     * @param isFlipped 是否翻转
     */
    void setFlipped(bool isFlipped);
    
    /// @}
};

} // namespace engine::render
