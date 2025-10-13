#pragma once
#include <SDL3/SDL_render.h>
#include <glm/vec2.hpp>

#include <vector>
#include <string>
#include <unordered_map>
#include <variant>

namespace engine::core {
    class Config;
} // namespace engine::core

namespace engine::input {

/**
 * @brief 表示输入动作的状态枚举
 * 
 * 这个枚举定义了游戏输入动作的四种可能状态，用于跟踪玩家输入的生命周期。
 */
enum class ActionState {
    INACTIVE,           ///< @brief 动作未激活
    PRESSED_THIS_FRAME, ///< @brief 动作在本帧刚刚被按下
    HELD_DOWN,          ///< @brief 动作被持续按下
    RELEASED_THIS_FRAME ///< @brief 动作在本帧刚刚被释放
};

/**
 * @brief 输入管理器类，负责处理输入事件和动作状态。
 * 
 * 该类管理输入事件，将按键转换为动作状态，并提供查询动作状态的功能。
 * 它还处理鼠标位置的逻辑坐标转换。
 * 
 * 使用方法：
 * @brief -  1. 创建 InputManager 实例，传入 SDL_Renderer 和 Config
 * @brief -  2. 在游戏循环中调用 update() 方法处理输入事件
 * @brief -  3. 使用 isActionDown(), isActionPressed(), isActionReleased() 检查动作状态
 * @brief -  4. 使用 getMousePosition() 和 getLogicalMousePosition() 获取鼠标位置
 * 
 * @note 该类是线程不安全的，不应在多线程环境中同时使用
 */
class InputManager final {
private:
    SDL_Renderer* m_SDLRenderer;           ///< @brief 用于获取逻辑坐标的 SDL_Renderer 指针

    std::unordered_map<std::string, std::vector<std::string>>                        m_actionsToKeynameMap;  ///< @brief 存储动作名称到按键名称列表的映射
    std::unordered_map<std::variant<SDL_Scancode, Uint32>, std::vector<std::string>> m_inputToActionsMap;    ///< @brief 从输入到关联的动作名称列表
    std::unordered_map<std::string, ActionState>                                     m_actionStates;         ///< @brief 存储每个动作的当前状态

    bool m_shouldQuit = false;             ///< @brief 请求退出游戏的标志
    glm::vec2 m_mousePosition;             ///< @brief 鼠标在屏幕坐标中的位置

public:
    /**
     * @brief 构造函数，初始化输入管理器
     * 
     * @param SDLRenderer SDL渲染器指针，用于获取逻辑坐标
     * @param config 配置对象指针，包含输入映射配置
     * 
     * @throws std::runtime_error 如果任意一个指针为空
     * 
     * @note 该构造函数会根据配置初始化输入映射表
     */
    InputManager(SDL_Renderer* SDLRenderer, const engine::core::Config* config);

    /**
     * @brief 更新输入管理器状态
     * 
     * 此方法应在每一帧开始时调用，用于处理所有待处理的SDL事件，
     * 更新动作状态，并重置"PRESSED_THIS_FRAME"和"RELEASED_THIS_FRAME"状态。
     * 
     * @note 必须在游戏循环中每帧调用此方法
     */
    void update();


    /// @name 动作状态检查
    /// @{
    /**
     * @brief 检查动作是否处于激活状态
     * 
     * 如果动作处于"HELD_DOWN"或"PRESSED_THIS_FRAME"状态，则返回true
     * 
     * @param actionName 要检查的动作名称
     * @return 如果动作处于激活状态则返回true，否则返回false
     * 
     * @note 此方法不会消耗动作状态，可以安全地多次调用
     */
    bool isActionDown(std::string_view actionName) const;        ///< @brief 动作当前是否触发 (持续按下或本帧按下)
    /**
     * @brief 检查动作是否在本帧刚刚被按下
     * 
     * 如果动作处于"PRESSED_THIS_FRAME"状态，则返回true
     * 
     * @param actionName 要检查的动作名称
     * @return 如果动作在本帧刚刚被按下则返回true，否则返回false
     * 
     * @note 此方法不会消耗动作状态，可以安全地多次调用
     */
    bool isActionPressed(std::string_view actionName) const;     ///< @brief 动作是否在本帧刚刚按下
    /**
     * @brief 检查动作是否在本帧刚刚被释放
     * 
     * 如果动作处于"RELEASED_THIS_FRAME"状态，则返回true
     * 
     * @param actionName 要检查的动作名称
     * @return 如果动作在本帧刚刚被释放则返回true，否则返回false
     * 
     * @note 此方法不会消耗动作状态，可以安全地多次调用
     */
    bool isActionReleased(std::string_view actionName) const;    ///< @brief 动作是否在本帧刚刚释放
    /// @}


    /// @name getter / setter
    /// @{
    /**
     * @brief 查询退出状态
     * 
     * @return 如果请求退出游戏则返回true，否则返回false
     */
    bool shouldQuit() const;                                         ///< @brief 查询退出状态
    /**
     * @brief 设置退出状态
     * 
     * @param shouldQuit 是否请求退出游戏
     */
    void setShouldQuit(bool shouldQuit);                            ///< @brief 设置退出状态
    /**
     * @brief 获取鼠标位置（屏幕坐标）
     * 
     * 返回鼠标在屏幕像素坐标中的位置
     * 
     * @return 鼠标屏幕坐标
     * 
     * @see getLogicalMousePosition
     */
    glm::vec2 getMousePosition() const;                              ///< @brief 获取鼠标位置 （屏幕坐标）
    /**
     * @brief 获取鼠标位置（逻辑坐标）
     * 
     * 返回鼠标在游戏世界逻辑坐标中的位置，考虑了渲染器的缩放和变换
     * 
     * @return 鼠标逻辑坐标
     * 
     * @see getMousePosition
     */
    glm::vec2 getLogicalMousePosition() const;                       ///< @brief 获取鼠标位置 （逻辑坐标）
    /// @}

private:
    /**
     * @brief 处理SDL事件
     * 
     * 将SDL事件转换为动作状态更新，包括键盘事件和鼠标事件
     * 
     * @param event SDL事件对象
     */
    void processEvent(const SDL_Event& event);                      ///< @brief 处理 SDL 事件（将按键转换为动作状态）
    
    /**
     * @brief 根据配置初始化映射表
     * 
     * 从配置对象中读取输入映射配置，建立动作到按键的映射关系
     * 
     * @param config 配置对象指针
     */
    void initializeMappings(const engine::core::Config* config);                            ///< @brief 根据 Config配置初始化映射表

    /**
     * @brief 更新指定动作的状态
     * 
     * 根据输入是否激活和是否为重复事件来更新动作状态
     * 
     * @param actionName 动作名称
     * @param isInputInput 是否激活
     * @param isRepeatEvent 是否为重复事件
     */
    void updateActionState(std::string_view actionName, bool isInputActive, bool isRepeatEvent); ///< @brief 辅助更新动作状态
    
    /**
     * @brief 将字符串键名转换为SDL_Scancode
     * 
     * @param keyName 键名称字符串
     * @return 对应的SDL_Scancode
     */
    SDL_Scancode scancodeFromString(std::string_view keyName);                           ///< @brief 将字符串键名转换为 SDL_Scancode
    
    /**
     * @brief 将字符串按钮名转换为SDL鼠标按钮码
     * 
     * @param buttonName 按钮名称字符串
     * @return 对应的SDL鼠标按钮码
     */
    Uint32 mouseButtonFromString(std::string_view buttonName);                       ///< @brief 将字符串按钮名转换为 SDL_Button
};

} // namespace engine::input
