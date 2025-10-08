#include "Sprite.hpp"

namespace engine::render {
Sprite::Sprite(const std::string_view textureID, const std::optional<SDL_FRect>& sourceRect, bool isFlipped)
    : m_textureID(textureID), m_sourceRect(sourceRect), m_isFlipped(isFlipped) { }


/// @name getter / setter
/// @{
const std::string_view Sprite::getTextureID() const {
    return m_textureID;
}

const std::optional<SDL_FRect> &Sprite::getSourceRect() const {
    return m_sourceRect;
}

bool Sprite::isFlipped() const {
    return m_isFlipped;
}

void Sprite::setFlipped(bool isFlipped) {
    m_isFlipped = isFlipped;
}

void Sprite::setSourceRect(const std::optional<SDL_FRect>& sourceRect) {
    m_sourceRect = sourceRect;
}

void Sprite::setTextureID(const std::string_view textureID) {
    m_textureID = textureID;
}
/// @}

} // namespace engine::render
