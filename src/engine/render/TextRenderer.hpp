#pragma once
#include <SDL3/SDL_render.h>
#include <string>
#include <string_view>
#include <glm/vec2.hpp>
#include "../utils/Math.hpp"

struct TTF_TextEngine;

namespace engine::resource {
    class ResourceManager;
}

namespace engine::render {
    class Camera;
/**
 * @brief 使用 SDL_ttf 和 TTF_Text 对象处理文本渲染。
 *
 * 封装 TTF_TextEngine 并提供创建和绘制 TTF_Text 对象的方法，
 * 管理字体加载和颜色设置。
 */
class TextRenderer final {
private:
    SDL_Renderer* m_SDLRenderer = nullptr;                          ///< @brief 持有渲染器的非拥有指针
    engine::resource::ResourceManager* m_resourceManager = nullptr; ///< @brief 持有资源管理器的非拥有指针
    
    TTF_TextEngine* m_textEngine = nullptr;         ///< @brief 使用SDL3引入的 TTF_TextEngine 来进行绘制

public:
    /**
     * @brief 构造 TextRenderer。
     *
     * @param SDLRenderer 有效的 SDLRenderer 指针。
     * @param resourceManager 有效的 ResourceManager 指针（用于字体加载）。
     * @throws std::runtime_error 如果初始化失败。
     */
    TextRenderer(SDL_Renderer* SDLRenderer, engine::resource::ResourceManager* resourceManager);

    ~TextRenderer();            ///< @brief 析构函数，按需调用close()。

    void close();               ///< @brief 显式关闭。清理 TTF_TextEngine 并关闭SDL_ttf。

    /**
     * @brief 绘制UI上的字符串。
     *        
     * @param text UTF-8 字符串内容。
     * @param fontID 字体 ID。
     * @param fontSize 字体大小。
     * @param position 左上角屏幕位置。
     * @param color 文本颜色。(默认为白色)
     */
    void drawUIText(std::string_view text, std::string_view fontID, int fontSize, const glm::vec2& position, const utils::FColor& color = {1.0f, 1.0f, 1.0f, 1.0f});

    /**
     * @brief 绘制地图上的字符串。
     *        
     * @param camera 相机
     * @param text UTF-8 字符串内容。
     * @param fontID 字体 ID。
     * @param fontSize 字体大小。
     * @param position 左上角屏幕位置。
     * @param color 文本颜色。
     */
    void drawText(const Camera& camera, std::string_view text, std::string_view fontID, int fontSize, const glm::vec2& position, const utils::FColor& color = {1.0f, 1.0f, 1.0f, 1.0f});

    /**
     * @brief 获取文本的尺寸。
     *
     * @param text 要测量的文本。
     * @param fontID 字体 ID。
     * @param fontSize 字体大小。
     * @return 文本的尺寸。
     */
    glm::vec2 getTextSize(std::string_view text, std::string_view fontID, int fontSize);

    // 禁用拷贝和移动语义
    TextRenderer(const TextRenderer&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;
    TextRenderer(TextRenderer&&) = delete;
    TextRenderer& operator=(TextRenderer&&) = delete;

}; // class TextRenderer

} // namespace engine::render