/**
 * @file Time.hpp
 * @brief 时间管理类，提供时间管理功能，包括时间间隔、时间缩放、帧率控制等
 */
#pragma once
#include <SDL3/SDL_stdinc.h>

namespace engine::core {
/** 
 * @brief 时间管理类
 * @brief 提供时间管理功能，包括时间间隔、时间缩放、帧率控制等
 */
class Time final {
private:
    /// @brief 时间相关
    /// @{
    Uint64 m_startTime = 0;    ///< @brief 开始时间
    Uint64 m_endTime   = 0;    ///< @brief 结束时间
    double m_deltaTime = 0.0;  ///< @brief 时间间隔
    double m_timeScale = 1.0;  ///< @brief 时间缩放
    /// @}

    /// @brief 帧率控制相关
    /// @{
    int    m_targrtFPS       = 60;    ///< @brief 目标帧率
    double m_targetFrameTime = 0.0;  ///< @brief 帧时间
    /// @}

public:
    Time();
    Time(int fps);

    ~Time();

    /// @name 禁用拷贝和移动
    /// @ {
    Time(const Time&) = delete;
    Time& operator=(const Time&) = delete;
    Time(Time&&) = delete;
    Time& operator=(Time&&) = delete;
    /// @ }

    /// @brief 更新时间
    void update();


    /// @name setters
    /// @{
    /**
     * @brief 设置目标帧率
     * @param fps 目标帧率
     */
    void setTargetFPS(int fps);
    /**
     * @brief 设置时间缩放
     * @param scale 时间缩放
     */
    void setTimeScale(double scale);
    /// @}


    /// @name getters
    /// @{
    /**
     * @brief 获取时间间隔
     * @return 时间间隔
     */
    double getDeltaTime() const;
    /**
     * @brief 获取未缩放的时间间隔
     * @return 未缩放的时间间隔
     */
    double getUnscaledDeltaTime() const;
    /**
     * @brief 获取时间缩放
     * @return 时间缩放
     */
    double getTimeScale() const;
    /**
     * @brief 获取帧时间
     * @return 帧时间
     */
    double getFrameTime() const;
    /// @}

private:
    /**
     * @brief 限制帧率
     * @param currentDeltaTime 当前时间间隔
     */
    void limitFrameRate(float currentDeltaTime);
    
};

} // namespace engine::core