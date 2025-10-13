#pragma once
#include "UIElement.hpp"
#include "../utils/Math.hpp"
#include "../render/TextRenderer.hpp"
#include <string>
#include <string_view>

namespace engine::ui {

/**
 * @brief UILabel 类用于创建和管理用户界面中的文本标签
 * 
 * UILabel 继承自 UIElement，提供了文本渲染功能。
 * 它可以设置文本内容、字体ID、字体大小和文本颜色。
 * 
 * @note 需要一个文本渲染器来获取和更新文本尺寸。
 */
class UILabel final : public UIElement {
private:
    engine::render::TextRenderer& m_textRenderer;   ///< @brief 需要文本渲染器，用于获取/更新文本尺寸
    
    std::string m_text;                          ///< @brief 文本内容    
    std::string m_fontID;                       ///< @brief 字体ID
    int m_fontSize;                             ///< @brief 字体大小   
    engine::utils::FColor m_textFColor = {1.0f, 1.0f, 1.0f, 1.0f};
    /* 可添加其他内容，例如边框、底色 */

public:
    /**
     * @brief 构造一个UILabel
     * 
     * @param textRenderer 文本渲染器
     * @param text 文本内容
     * @param fontID 字体ID
     * @param fontSize 字体大小
     * @param textColor 文本颜色
     */
    UILabel(
        engine::render::TextRenderer& textRenderer,
        std::string_view text,
        std::string_view fontID,
        int fontSize = 16,
        engine::utils::FColor textColor = {1.0f, 1.0f, 1.0f, 1.0f},
        glm::vec2 position = {0.0f, 0.0f}
    );

    // --- 核心方法 ---
    void render(engine::core::Context& context) override;

    // --- Setters & Getters ---
    std::string_view getText() const { return m_text; }
    std::string_view getFontId() const { return m_fontID; }
    int getFontSize() const { return m_fontSize; }
    const engine::utils::FColor& getTextFColor() const { return m_textFColor; }

    void setText(std::string_view text);                      ///< @brief 设置文本内容, 同时更新尺寸
    void setFontId(std::string_view fontID);                 ///< @brief 设置字体ID, 同时更新尺寸
    void setFontSize(int fontSize);                            ///< @brief 设置字体大小, 同时更新尺寸
    void setTextFColor(engine::utils::FColor textFColor);

};


} // namespace engine::ui