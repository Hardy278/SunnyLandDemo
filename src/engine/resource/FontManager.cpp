#include "FontManager.hpp"

#include <SDL3_ttf/SDL_ttf.h>
#include <spdlog/spdlog.h>

#include <stdexcept>
#include <utility>

namespace engine::resource {

std::size_t FontKeyHash::operator()(const FontKey &key) const {
    std::hash<std::string> stringHasher;
    std::hash<int> intHasher;
    return stringHasher(key.first) ^ (intHasher(key.second) << 1);
}

void FontManager::SDLFontDeleter::operator()(TTF_Font *font) const {
    if (font) {
        TTF_CloseFont(font);
    }
}

FontManager::FontManager() {
    if(!TTF_WasInit() && !TTF_Init()) {
        throw std::runtime_error("RESOURCEMANAGER::FONTMANAGER::初始化 SDL_ttf 失败: " + std::string(SDL_GetError()));
    }
    spdlog::trace("RESOURCEMANAGER::FONTMANAGER::FontManager初始化成功");
}

FontManager::~FontManager() {
    if (!m_fonts.empty()) {
        spdlog::warn("RESOURCEMANAGER::FONTMANAGER::警告: {} 个字体未被卸载", m_fonts.size());
        clearFonts();
    }
    TTF_Quit();
    spdlog::trace("RESOURCEMANAGER::FONTMANAGER::FontManager退出成功");
}

/// @name loader / unloader / getter
/// @{
TTF_Font *FontManager::loadFont(const std::string_view path, int size) {
    if (size <= 0) {
        spdlog::error("RESOURCEMANAGER::FONTMANAGER::loadFont::无法加载字体\"{}\": 无效的字体大小: {}", path, size);
        return nullptr;
    }
    FontKey key(std::string(path), size);

    auto it = m_fonts.find(key);
    if (it != m_fonts.end()) {
        spdlog::warn("RESOURCEMANAGER::FONTMANAGER::loadFont::字体\"{}\"已存在", path);
        return it->second.get();
    }

    TTF_Font *font = TTF_OpenFont(std::string(path).c_str(), static_cast<float>(size));
    if (!font) {
        spdlog::error("RESOURCEMANAGER::FONTMANAGER::loadFont::无法加载字体\"{}\"({}pt): {}", path, size, SDL_GetError());
        return nullptr;
    }
    m_fonts.emplace(key, std::unique_ptr<TTF_Font, SDLFontDeleter>(font));
    spdlog::debug("RESOURCEMANAGER::FONTMANAGER::loadFont::字体\"{}\"({}pt)加载成功", path, size);
    return font;
}

TTF_Font *FontManager::getFont(const std::string_view path, int size) {
    FontKey key = { std::string(path), size };
    auto it = m_fonts.find(key);
    if (it != m_fonts.end()) {
        return it->second.get();
    }
    spdlog::error("RESOURCEMANAGER::FONTMANAGER::getFont::字体\"{}\"({}pt)不存在, 尝试加载", path, size);
    return loadFont(path, size);
}

void FontManager::unloadFont(const std::string_view path, int size) {
    FontKey key = { std::string(path), size };
    auto it = m_fonts.find(key);
    if (it != m_fonts.end()) {
        spdlog::debug("RESOURCEMANAGER::FONTMANAGER::unloadFont::字体\"{}\"({}pt)已卸载", path, size);
        m_fonts.erase(it);
    } else {    
        spdlog::warn("RESOURCEMANAGER::FONTMANAGER::unloadFont::字体\"{}\"({}pt)不存在", path, size);
    }
}

void FontManager::clearFonts() {
    if (!m_fonts.empty()) {
        spdlog::debug("RESOURCEMANAGER::FONTMANAGER::clearFonts::所有 {} 个字体已卸载", m_fonts.size());
        m_fonts.clear();
    }
}
/// @}
}