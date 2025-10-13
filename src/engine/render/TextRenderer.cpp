#include "TextRenderer.hpp"
#include "Camera.hpp"
#include "../resource/ResourceManager.hpp"
#include <SDL3_ttf/SDL_ttf.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::render {

TextRenderer::TextRenderer(SDL_Renderer* SDLRenderer, engine::resource::ResourceManager* resourceManager)
    : m_SDLRenderer(SDLRenderer), m_resourceManager(resourceManager) {
    if (!m_SDLRenderer || !m_resourceManager) {
        throw std::runtime_error("TextRenderer 需要一个有效的 SDLRenderer 和 ResourceManager。");
    }
    // 初始化 SDL_ttf
    if (!TTF_WasInit() && TTF_Init() == false) {
         throw std::runtime_error("初始化 SDL_ttf 失败: " + std::string(SDL_GetError()));
    }

    m_textEngine = TTF_CreateRendererTextEngine(m_SDLRenderer);
    if (!m_textEngine) {
        spdlog::error("创建 TTF_TextEngine 失败: {}", SDL_GetError());
        throw std::runtime_error("创建 TTF_TextEngine 失败。");
    }
    spdlog::trace("TextRenderer 初始化成功.");
}

TextRenderer::~TextRenderer() {
    if (m_textEngine) {
        close();
    }
}

void TextRenderer::close() {
    if (m_textEngine) {
        TTF_DestroyRendererTextEngine(m_textEngine);
        m_textEngine = nullptr;
        spdlog::trace("TTF_TextEngine 销毁。");
    }
    TTF_Quit();     // 一定要确保在ResourceManager销毁之后调用
}

void TextRenderer::drawUIText(std::string_view text, std::string_view fontID, int fontSize, const glm::vec2 &position, const utils::FColor &color) {
    /* 构造函数已经保证了必要指针不会为空，这里不需要再检查 */
    TTF_Font* font = m_resourceManager->getFont(fontID, fontSize);
    if (!font) {
        spdlog::warn("drawUIText 获取字体失败: {} 大小 {}", fontID, fontSize);
        return;
    }

    // 创建临时 TTF_Text 对象   (目前效率不高，未来可以考虑使用缓存优化)
    TTF_Text* tempTextObject = TTF_CreateText(m_textEngine, font, text.data(), 0);
    if (!tempTextObject) {
        spdlog::error("drawUIText 创建临时 TTF_Text 失败: {}", SDL_GetError());
        return;
    }

    // 先渲染一次黑色文字模拟阴影
    TTF_SetTextColorFloat(tempTextObject, 0.0f, 0.0f, 0.0f, 1.0f);
    if (!TTF_DrawRendererText(tempTextObject, position.x + 2, position.y + 2)) {
        spdlog::error("drawUIText 绘制临时 TTF_Text 失败: {}", SDL_GetError());
    }

    // 然后正常绘制
    TTF_SetTextColorFloat(tempTextObject, color.r, color.g, color.b, color.a);
    if (!TTF_DrawRendererText(tempTextObject, position.x, position.y)) {
        spdlog::error("drawUIText 绘制临时 TTF_Text 失败: {}", SDL_GetError());
    }

    // 销毁临时 TTF_Text 对象
    TTF_DestroyText(tempTextObject);
}

void TextRenderer::drawText(const Camera &camera, std::string_view text, std::string_view fontID, int fontSize, const glm::vec2 &position, const utils::FColor &color) {
    // 应用相机变换
    glm::vec2 positionScreen = camera.worldToScreen(position);
    // 用新坐标调用drawUIText即可
    drawUIText(text, fontID, fontSize, positionScreen, color);
}

glm::vec2 TextRenderer::getTextSize(std::string_view text, std::string_view fontID, int fontSize) {
    /* 构造函数已经保证了必要指针不会为空，这里不需要再检查 */
    TTF_Font* font = m_resourceManager->getFont(fontID, fontSize);
    if (!font) {
        spdlog::warn("getTextSize 获取字体失败: {} 大小 {}", fontID, fontSize);
        return glm::vec2(0.0f, 0.0f);
    }

    // 创建临时 TTF_Text 对象
    TTF_Text* tempTextObject = TTF_CreateText(m_textEngine, font, text.data(), 0);
    if (!tempTextObject) {
        spdlog::error("getTextSize 创建临时 TTF_Text 失败: {}", SDL_GetError());
        return glm::vec2(0.0f, 0.0f);
    }

    int width, height;
    TTF_GetTextSize(tempTextObject, &width, &height);

    // 销毁临时 TTF_Text 对象
    TTF_DestroyText(tempTextObject);

    return glm::vec2(static_cast<float>(width), static_cast<float>(height));
} 

} // namespace engine::render 