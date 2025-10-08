/**
 * @file Scene.h
 * @brief 场景管理系统的核心类定义
 */
#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <memory>

namespace engine::core {
class Context;
} // namespace engine::core 

namespace engine::render {
class Renderer;
class Camera;
} // namespace engine::render

namespace engine::object {
class GameObject;
} // namespace engine::object

namespace engine::input {
class InputManager;
} // namespace engine::input

namespace engine::scene {
class SceneManager;

/**
 * @class Scene
 * @brief 场景类，用于管理游戏场景中的所有游戏对象和场景生命周期
 * 
 * Scene 类是游戏场景的基本单位，负责管理场景中的所有游戏对象，
 * 处理场景的初始化、更新、渲染和清理等生命周期事件。
 */
class Scene {
protected:
    std::string m_sceneName;                            ///< 场景的唯一标识名称
    engine::core::Context& m_context;                   ///< 引用引擎的核心上下文，提供全局服务
    engine::scene::SceneManager& m_sceneManager;        ///< 引用场景管理器，用于场景切换和管理
    
    bool m_isInitialized;                                ///< 场景初始化状态标志，防止重复初始化
    std::vector<std::unique_ptr<engine::object::GameObject>> m_gameObjects;         ///< 当前场景中活跃的游戏对象列表
    std::vector<std::unique_ptr<engine::object::GameObject>> m_pendingAdditions;    ///< 等待添加到场景的游戏对象队列

public:
    /**
     * @brief 构造函数
     * @param name 场景名称
     * @param context 引擎上下文引用
     * @param sceneManager 场景管理器引用
     */
    Scene(std::string_view name, engine::core::Context& context, engine::scene::SceneManager& sceneManager);
    /**
     * @brief 虚析构函数
     * 
     * 声明为虚函数以确保派生类的析构函数能被正确调用。
     * 实现定义在cpp文件中以避免包含GameObject头文件。
     */
    virtual ~Scene();

    /// @name 禁止拷贝和移动构造
    /// @{
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(Scene&&) = delete;
    /// @}


    /// @name 生命周期管理
    /// @{
    /**
     * @brief 初始化场景
     * 
     * 在场景第一次激活时调用，用于初始化场景资源和游戏对象。
     * 派生类应该重写此方法以实现特定的初始化逻辑。
     */
    virtual void init();
    /**
     * @brief 更新场景状态
     * @param deltaTime 距离上一帧的时间（秒）
     * 
     * 每帧调用一次，用于更新场景中所有游戏对象的状态。
     */
    virtual void update(float deltaTime);
    /**
     * @brief 渲染场景
     * 
     * 每帧调用一次，用于渲染场景中的所有可见对象。
     */
    virtual void render();
    /**
     * @brief 处理输入事件
     * 
     * 处理场景相关的用户输入事件。
     */
    virtual void handleInput();
    /**
     * @brief 清理场景资源
     * 
     * 在场景被销毁或切换时调用，用于释放场景资源。
     */
    virtual void clean();
    /// @}


    /// @name 游戏对象管理
    /// @{
    /**
     * @brief 直接添加游戏对象到场景
     * @param game_object 要添加的游戏对象的右值引用
     * 
     * 注意：此方法仅在场景初始化时使用是安全的。
     * 在游戏运行时使用可能导致迭代器失效。
     */
    virtual void addGameObject(std::unique_ptr<engine::object::GameObject>&& gameObject);
    /**
     * @brief 安全地添加游戏对象到场景
     * @param game_object 要添加的游戏对象的右值引用
     * 
     * 将游戏对象添加到待添加队列中，在下一帧开始前统一处理。
     * 这是运行时添加游戏对象的推荐方式。
     */
    virtual void safeAddGameObject(std::unique_ptr<engine::object::GameObject>&& gameObject);
    /**
     * @brief 直接从场景中移除游戏对象
     * @param game_object_ptr 要移除的游戏对象指针
     * 
     * 注意：此方法不安全，可能导致迭代器失效。
     * 建议使用 safeRemoveGameObject 代替。
     */
    virtual void removeGameObject(engine::object::GameObject* gameObjectPtr);
    /**
     * @brief 安全地从场景中移除游戏对象
     * @param game_object_ptr 要移除的游戏对象指针
     * 
     * 标记游戏对象为待移除状态，在适当的时候安全地移除。
     */
    virtual void safeRemoveGameObject(engine::object::GameObject* gameObjectPtr);
    /**
     * @brief 获取场景中的游戏对象容器
     * @return 游戏对象容器的常量引用
     */
    const std::vector<std::unique_ptr<engine::object::GameObject>>& getGameObjects() const;
    /**
     * @brief 根据名称查找游戏对象
     * @param name 要查找的游戏对象名称
     * @return 找到的第一个匹配的游戏对象指针，未找到返回nullptr
     */
    engine::object::GameObject* findGameObjectByName(std::string_view name) const;
    /// @}


    /// @name Getters and Setters
    /// @{
    /**
     * @brief 设置场景名称
     * @param name 新的场景名称
     */
    void setName(std::string_view name);
    /**
     * @brief 设置场景初始化状态
     * @param initialized 初始化状态
     */
    void setInitialized(bool initialized);

    /**
     * @brief 获取场景初始化状态
     * @return 如果场景已初始化返回true，否则返回false
     */
    bool isInitialized() const;
    /**
     * @brief 获取场景名称
     * @return 场景名称的字符串视图
     */
    std::string_view getName() const;
    /**
     * @brief 获取引擎上下文引用
     * @return 引擎上下文引用
     */
    engine::core::Context& getContext() const;
    /**
     * @brief 获取场景管理器引用
     * @return 场景管理器引用
     */
    engine::scene::SceneManager& getSceneManager() const;
    /**
     * @brief 获取场景中的游戏对象容器
     * @return 游戏对象容器的引用
     */
    std::vector<std::unique_ptr<engine::object::GameObject>>& getGameObjects();
    /// @}

protected:
    /**
     * @brief 处理待添加的游戏对象
     * 
     * 将待添加队列中的所有游戏对象添加到场景中。
     * 通常在每帧更新的最后调用。
     */
    void processPendingAdditions();
};

} // namespace engine::scene
