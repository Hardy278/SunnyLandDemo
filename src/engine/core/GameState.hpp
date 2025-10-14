#pragma once
#include <optional>
#include <string>
#include <glm/vec2.hpp>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>

namespace engine::core {

/**
 * @enum State
 * @brief 定义游戏可能处于的宏观状态。
 */
enum class State {
    Title,          ///< @brief 标题界面
    Playing,        ///< @brief 正常游戏进行中
    Paused,         ///< @brief 游戏暂停（通常覆盖菜单界面）
    GameOver,       ///< @brief 游戏结束界面
    // 可以根据需要添加更多状态，如 Cutscene, SettingsMenu 等
};

/**
 * @brief 管理和查询游戏的全局宏观状态。
 *
 * 提供一个中心点来确定游戏当前处于哪个主要模式，
 * 以便其他系统（输入、渲染、更新等）可以相应地调整其行为。
 */
class GameState final {
private:    
    SDL_Window*   m_window       = nullptr;            ///< @brief SDL窗口，用于获取窗口大小
    SDL_Renderer* m_renderer     = nullptr;            ///< @brief SDL渲染器，用于获取逻辑分辨率
    State         m_currentState = State::Title;       ///< @brief 当前游戏状态

public:
    /**
     * @brief 构造函数，初始化游戏状态。
     * @param window SDL窗口，必须传入有效值。
     * @param renderer SDL渲染器，必须传入有效值。
     * @param initialState 游戏的初始状态，默认为 Title
     */
    explicit GameState(SDL_Window* window, SDL_Renderer* renderer, State initialState = State::Title);

    State getCurrentState() const { return m_currentState; }
    glm::vec2 getLogicalSize() const;
    glm::vec2 getWindowSize() const;
    void setState(State newState);
    void setWindowSize(const glm::vec2& m_windowsize);   // 这里并不涉及到(成员变量)赋值，所以不需要move
    void setLogicalSize(const glm::vec2& logicalSize);


    // --- 便捷查询方法 ---
    bool isInTitle() const { return m_currentState == State::Title; }
    bool isPlaying() const { return m_currentState == State::Playing; }
    bool isPaused() const { return m_currentState == State::Paused; }
    bool isGameOver() const { return m_currentState == State::GameOver; }

};

} // namespace engine::core