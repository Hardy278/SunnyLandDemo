#include "UILabel.hpp"
#include "../core/Context.hpp"
#include "../render/TextRenderer.hpp"
#include <spdlog/spdlog.h>

namespace engine::ui {

UILabel::UILabel(
    engine::render::TextRenderer& textRenderer,
    std::string_view text,
    std::string_view fontID,
    int fontSize,
    engine::utils::FColor textColor,
    glm::vec2 position
) : UIElement(std::move(position)), m_textRenderer(textRenderer), m_text(text), m_fontID(fontID), m_fontSize(fontSize), m_textFColor(std::move(textColor)) {
    // 获取文本渲染尺寸
    m_size = m_textRenderer.getTextSize(m_text, m_fontID, m_fontSize);
    spdlog::trace("UILABEL::UILabel 构造完成");
}

void UILabel::render(engine::core::Context& context) {
    if (!m_visible || m_text.empty()) return;
    m_textRenderer.drawUIText(m_text, m_fontID, m_fontSize, getScreenPosition(), m_textFColor);
    // 渲染子元素（调用基类方法）
    UIElement::render(context);
}

void UILabel::setText(std::string_view text) {
    m_text = text;
    m_size = m_textRenderer.getTextSize(m_text, m_fontID, m_fontSize);
}

void UILabel::setFontId(std::string_view fontID) {
    m_fontID = fontID;
    m_size = m_textRenderer.getTextSize(m_text, m_fontID, m_fontSize);
}

void UILabel::setFontSize(int fontSize) {
    m_fontSize = fontSize;
    m_size = m_textRenderer.getTextSize(m_text, m_fontID, m_fontSize);
}

void UILabel::setTextFColor(engine::utils::FColor textFcolor) {
    m_textFColor = std::move(textFcolor);
    /* 颜色变化不影响尺寸 */
}

} // namespace engine::ui