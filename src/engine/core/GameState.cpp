#include "GameState.hpp"
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace engine::core {

GameState::GameState(SDL_Window* window, SDL_Renderer* renderer, State initialState)
    : m_window(window), m_renderer(renderer), m_currentState(initialState){
    if (m_window == nullptr || m_renderer == nullptr) {
        spdlog::error("GAMESTATE::窗口或渲染器为空");
        throw std::runtime_error("GAMESTATE::窗口或渲染器不能为空");
    }
    spdlog::trace("GAMESTATE::游戏状态初始化完成");
}

void GameState::setState(State newState) {
    if (m_currentState != newState) {
        spdlog::debug("GAMESTATE::setState::游戏状态改变");
        m_currentState = std::move(newState);
    } else {
        spdlog::debug("GAMESTATE::setState::尝试设置相同的游戏状态，跳过");
    }
}

glm::vec2 GameState::getWindowSize() const {
    int width, height;
    // SDL3获取窗口大小的方法
    SDL_GetWindowSize(m_window, &width, &height);
    return glm::vec2(width, height);
}

void GameState::setWindowSize(const glm::vec2& m_windowsize) {
    SDL_SetWindowSize(m_window, static_cast<int>(m_windowsize.x), static_cast<int>(m_windowsize.y));
}

glm::vec2 GameState::getLogicalSize() const {
    int width, height;
    // SDL3获取逻辑分辨率的方法
    SDL_GetRenderLogicalPresentation(m_renderer, &width, &height, NULL);
    return glm::vec2(width, height);
}

void GameState::setLogicalSize(const glm::vec2& logicalSize) {
    SDL_SetRenderLogicalPresentation(m_renderer, static_cast<int>(logicalSize.x), static_cast<int>(logicalSize.y), SDL_LOGICAL_PRESENTATION_LETTERBOX);
    spdlog::trace("GAMESTATE::setLogicalSize::逻辑分辨率设置为: {}x{}", logicalSize.x, logicalSize.y);
}


} // namespace engine::core 