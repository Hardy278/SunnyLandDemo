/**
 * @file Game.h
 * @brief 游戏引擎核心模块中的游戏类定义
 */

#pragma once
#include <memory>


/// @name 前向声明
/// @{
struct SDL_Window;
struct SDL_Renderer;

namespace engine::resource {
class ResourceManager;
} // namespace engine::resource

namespace engine::render {
class Renderer;
class Camera;
} // namespace engine::render

namespace engine::input {
class InputManager;
} // namespace engine::input

namespace engine::scene {
class SceneManager;
}

namespace engine::physics {
class PhysicsEngine;
}
/// @}


namespace engine::core {

class Time; // 前向声明
class Config;
class Context;

/**
 * @class Game
 * @brief 游戏主循环和资源管理的核心类
 *
 * 这个类负责初始化SDL窗口和渲染器，处理事件循环，
 * 更新游戏状态，并渲染图形。它还禁用了拷贝和移动操作。
 */
class Game final {
private:   
    /// @name 游戏资源
    /// @{
    SDL_Window   *m_window = nullptr;      /**< 指向SDL窗口的指针 */
    SDL_Renderer *m_SDLRenderer = nullptr; /**< 指向SDL渲染器的指针 */
    bool          m_isRunning = false;     /**< 标记游戏是否正在运行 */
    /// @}

    /// @name 游戏组件
    /// @{
    std::unique_ptr<Time>                      m_time = nullptr;             /**< 指向时间管理组件的智能指针 */
    std::unique_ptr<resource::ResourceManager> m_resourceManager = nullptr;  /**< 指向资源管理组件的智能指针 */
    std::unique_ptr<render::Renderer>          m_renderer = nullptr;         /**< 指向渲染器组件的智能指针 */
    std::unique_ptr<render::Camera>            m_camera = nullptr;           /**< 指向相机组件的智能指针 */
    std::unique_ptr<Config>                    m_config = nullptr;           /**< 指向配置文件的智能指针 */
    std::unique_ptr<input::InputManager>       m_inputManager = nullptr;     /**< 指向输入管理组件的智能指针 */
    std::unique_ptr<Context>                   m_context = nullptr;          /**< 指向游戏上下文的智能指针 */
    std::unique_ptr<scene::SceneManager>       m_sceneManager = nullptr;     /**< 指向场景管理器的智能指针 */
    std::unique_ptr<physics::PhysicsEngine>    m_physicsEngine = nullptr;    /**< 指向物理引擎的智能指针 */
    /// @}

public:
    Game();
    ~Game();

    /// @brief 启动游戏主循环
    void run();

    /// @name 禁用拷贝和移动
    /// @ {
    Game(const Game &) = delete;            /**< 删除拷贝构造函数 */
    Game &operator=(const Game &) = delete; /**< 删除拷贝赋值运算符 */
    Game(Game &&) = delete;                 /**< 删除移动构造函数 */
    Game &operator=(Game &&) = delete;      /**< 删除移动赋值运算符 */
    /// @ }

private:
    /// @name 游戏主要函数
    /// @{
    /**
     * @brief 初始化SDL窗口和渲染器
     * @return 成功返回true，失败返回false
     */
    [[nodiscard]] bool init();
    void handleEvents();    /// @brief 处理SDL事件
    /**
     * @brief 更新游戏状态
     * @param deltaTime 上一帧到当前帧的时间间隔（秒）
     */
    void update(float deltaTime);
    void render();    /// @brief 渲染游戏画面
    void close();    /// @brief 关闭SDL窗口和渲染器，释放资源
    /// @}

    
    /// @name 游戏组件管理
    /// @{
    [[nodiscard]] bool initConfig();             /// @brief 初始化配置类
    [[nodiscard]] bool initWindow();             /// @brief 初始化SDL窗口
    [[nodiscard]] bool initTime();               /// @brief 初始化时间管理组件
    [[nodiscard]] bool initResourceManager();    /// @brief 初始化资源管理组件
    [[nodiscard]] bool initRenderer();           /// @brief 初始化渲染器组件
    [[nodiscard]] bool initCamera();             /// @brief 初始化相机组件
    [[nodiscard]] bool initInputManager();       /// @brief 初始化输入管理组件
    [[nodiscard]] bool initPhysicsEngine();      /// @brief 初始化物理引擎
    [[nodiscard]] bool initContext();            /// @brief 初始化游戏上下文
    [[nodiscard]] bool initSceneManager();       /// @brief 初始化场景管理器
    /// @}
};

} // namespace engine::core


