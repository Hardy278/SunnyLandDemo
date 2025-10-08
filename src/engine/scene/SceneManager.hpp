/**
 * @file SceneManager.h
 * @brief 场景管理器的实现，负责场景的切换和管理
 */

#pragma once
#include <memory>
#include <string>
#include <vector>

// 前置声明
namespace engine::core {
    class Context;
}
namespace engine::scene {
    class Scene;
}

namespace engine::scene {

/**
 * @class SceneManager
 * @brief 场景管理器类，用于管理游戏场景的生命周期和切换
 * 
 * SceneManager 使用栈结构管理场景，支持场景的压入、弹出和替换操作。
 * 所有场景切换操作都是线程安全的，使用延时处理机制确保场景切换不会中断当前帧的渲染。
 * 该类被声明为 final，禁止被继承。
 */
class SceneManager final {
private:
    /**
     * @enum PendingAction
     * @brief 待处理的场景操作类型
     */
    enum class PendingAction { 
        None,       ///< 无待处理操作
        Push,       ///< 待压入新场景
        Pop,        ///< 待弹出当前场景
        Replace     ///< 待替换当前场景
    };

    engine::core::Context& m_context;                       ///< 引擎上下文引用，提供全局系统服务
    std::vector<std::unique_ptr<Scene>> m_sceneStack;       ///< 场景栈，存储当前所有活动场景

    PendingAction m_pendingAction;                          ///< 当前待处理的场景操作类型
    std::unique_ptr<Scene> m_pendingScene;                  ///< 待处理的新场景（用于Push和Replace操作）

public:
    explicit SceneManager(engine::core::Context& context);
    ~SceneManager();

    
    /// @name 禁止拷贝和移动
    /// @{
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    SceneManager(SceneManager&&) = delete;
    SceneManager& operator=(SceneManager&&) = delete;
    /// @}


    /// @name 场景管理器请求
    /// @{
    /**
     * @brief 请求压入一个新场景
     * @param scene 要压入的场景的唯一指针
     * 
     * 将新场景压入场景栈，使其成为当前活动场景。
     * 操作会在下一帧开始前执行。
     */
    void requestPushScene(std::unique_ptr<Scene>&& scene);
    /**
     * @brief 请求弹出当前场景
     * 
     * 从场景栈中移除当前活动场景，使栈顶的下一个场景成为活动场景。
     * 如果场景栈为空，则不做任何操作。
     * 操作会在下一帧开始前执行。
     */
    void requestPopScene();
    /**
     * @brief 请求替换当前场景
     * @param scene 要替换的新场景的唯一指针
     * 
     * 清理场景栈中的所有场景，并将新场景压入栈中。
     * 操作会在下一帧开始前执行。
     */
    void requestReplaceScene(std::unique_ptr<Scene>&& scene);
    /// @}


    /// @name getter
    /// @{
    /**
     * @brief 获取当前活动场景
     * @return 当前活动场景（栈顶场景）的指针，如果没有场景则返回nullptr
     */
    Scene* getCurrentScene() const;
    /**
     * @brief 获取引擎上下文引用
     * @return 引擎上下文引用
     */
    engine::core::Context& getContext() const;
    /// @}


    /// @name 场景管理器生命周期
    /// @{
    /**
     * @brief 更新场景状态
     * @param deltaTime 距离上一帧的时间（秒）
     * 
     * 更新当前活动场景的状态
     * 在更新开始前会处理所有待处理的场景操作
     */
    void update(float deltaTime);
    /**
     * @brief 渲染当前场景
     * 
     * 渲染当前活动场景中的所有可见对象
     */
    void render();
    /**
     * @brief 处理输入事件
     * 
     * 将输入事件传递给当前活动场景处理
     */
    void handleInput();
    /**
     * @brief 关闭场景管理器
     * 
     * 清理所有场景并释放资源
     */
    void close();
    /// @}

private:
    /**
     * @brief 处理挂起的场景操作
     * 
     * 执行所有待处理的场景切换操作（压入、弹出或替换）。
     * 在每帧更新开始时调用。
     */
    void processPendingActions();
    /**
     * @brief 直接压入新场景
     * @param scene 要压入的场景的唯一指针
     * 
     * 立即将新场景压入场景栈。
     * @note 此方法直接操作场景栈，可能导致不安全的状态，应仅在 processPendingActions 中使用。
     */
    void pushScene(std::unique_ptr<Scene>&& scene);
    /**
     * @brief 直接弹出当前场景
     * 
     * 立即从场景栈中移除当前活动场景。
     * @note 此方法直接操作场景栈，可能导致不安全的状态，应仅在 processPendingActions 中使用。
     */
    void popScene();
    /**
     * @brief 直接替换当前场景
     * @param scene 要替换的新场景的唯一指针
     * 
     * 立即清理场景栈中的所有场景，并将新场景压入栈中。
     * @note 此方法直接操作场景栈，可能导致不安全的状态，应仅在 processPendingActions 中使用。
     */
    void replaceScene(std::unique_ptr<Scene>&& scene);
};

} // namespace engine::scene
