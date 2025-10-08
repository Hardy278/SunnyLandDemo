#pragma once
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <functional>

// 前向声明
struct TTF_Font;

namespace engine::resource {
    using FontKey = std::pair<std::string, int>;
/**
 * @struct FontKeyHash
 * @brief 字体键哈希函数
 * @note 用于unordered_map的哈希函数
 */
struct FontKeyHash {
    std::size_t operator()(const FontKey& key) const;
};

/**
 * @class FontManager
 * @brief 字体管理器
 */
class FontManager final {
    friend class ResourceManager;
private:
    /**
     * @struct SDLFontDeleter
     * @brief SDL字体删除器
     * @note 用于unique_ptr的删除器
     */
    struct SDLFontDeleter {
        void operator()(TTF_Font* font) const;
    };

    std::unordered_map<FontKey, std::unique_ptr<TTF_Font, SDLFontDeleter>, FontKeyHash> m_fonts; ///< @brief 字体映射表
public:
    FontManager();
    ~FontManager();

    /// @name 删除拷贝移动构造函数
    /// @{
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;
    FontManager(FontManager&&) = delete;
    FontManager& operator=(FontManager&&) = delete;
    /// @}

private:
    /// @name loader / unloader / getter
    /// @{
    /**
     * @brief 加载字体
     * @param path 字体路径
     * @return TTF_Font* 字体指针
     */
    TTF_Font* loadFont(const std::string_view path, int size);
    /**
     * @brief 获取字体
     * @param path 字体路径
     * @param size 字体大小
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

};
} // namespace engine::resource