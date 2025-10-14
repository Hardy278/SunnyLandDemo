#pragma once
#include "../component/Component.hpp"

#include <spdlog/spdlog.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <typeindex>
#include <utility>

namespace engine::core {
class Context;
} // namespace engine::core

namespace engine::object {

/**
 * @brief 游戏对象类，用于管理游戏中的实体对象
 * 该类使用组件模式管理功能，支持添加、获取、移除组件
 */
class GameObject final {
private:
    bool        m_needRemove = false; ///< @brief 延迟删除的标识，将来由场景类负责删除
    std::string m_name;               /// @brief 对象名称
    std::string m_tag;                /// @brief 对象标签
    
    std::unordered_map<std::type_index, std::unique_ptr<component::Component>> m_components; ///< @brief 组件列表

public:
    /**
     * @brief 构造函数
     * @param name 对象名称，默认为空
     * @param tag 对象标签，默认为空
     */
    GameObject(std::string_view name = "", std::string_view tag = "");

    /// @name 删除移动拷贝函数
    /// @{ 
    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;
    GameObject(GameObject&&) = delete;
    GameObject& operator=(GameObject&&) = delete;
    /// @}

    /// @name setter / getter
    /// @{
    void setName(std::string_view name) { m_name = name; }
    void setTag(std::string_view tag) { m_tag = tag; }
    void setNeedRemove(bool needRemove) { m_needRemove = needRemove; }
    std::string_view getName() const { return m_name; }
    std::string_view getTag() const { return m_tag; }
    bool isNeedRemove() const { return m_needRemove; }
    /// @}

    /// @name 组件管理
    /// @{
    /**
     * @brief 添加组件 (里面会完成组件的init())
     * @tparam T 组件类型
     * @tparam Args 组件构造函数参数类型
     * @param args 组件构造函数参数
     * @return 组件指针
     */
    template <typename T, typename... Args>
    inline T *addComponent(Args &&...args) {
        // 检测组件是否合法。  
        /* static_assert(condition, message)：静态断言，在编译期检测，无任何性能影响 */
        /* std::is_base_of<Base, Derived>::value -- 判断 Base 类型是否是 Derived 类型的基类 */
        static_assert(std::is_base_of<engine::component::Component, T>::value, "GAMEOBJECT::addComponent::ERROR::T 必须继承自 Component");
        // 获取类型标识。     
        /* typeid(T) -- 用于获取一个表达式或类型的运行时类型信息 (RTTI), 返回 std::type_info& */
        /* std::type_index -- 针对std::type_info对象的包装器，主要设计用来作为关联容器（如 std::map）的键。*/
        auto typeIndex = std::type_index(typeid(T));
        // 如果组件已经存在，则直接返回组件指针
        if (hasComponent<T>()) {
            return getComponent<T>();
        }
        // 如果不存在则创建组件     /* std::forward -- 用于实现完美转发。传递多个参数的时候使用...标识 */
        auto newComponent = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = newComponent.get();                               // 先获取裸指针以便返回
        newComponent->setOwner(this);                              // 设置组件的拥有者
        m_components[typeIndex] = std::move(newComponent);         // 移动组件   （newComponent 变为空，不可再使用）
        ptr->init();                                               // 初始化组件 （因此必须用ptr而不能用newComponent）
        spdlog::debug("GAMEOBJECT::addComponent::{} 添加组件 {}", m_name, typeid(T).name());
        return ptr;  
    }
    /**
     * @brief 获取组件
     * @tparam T 组件类型
     * @return 组件指针
     */
    template <typename T>
    T *getComponent() const {
        static_assert(std::is_base_of<engine::component::Component, T>::value, "GAMEOBJECT::getComponent::ERROR::T 必须继承自 Component");
        auto typeIndex = std::type_index(typeid(T));
        auto it = m_components.find(typeIndex);
        if (it != m_components.end()) {
            // 返回unique_ptr的裸指针。(肯定是T类型, static_cast其实并无必要，但保留可以使我们意图更清晰)
            return static_cast<T*>(it->second.get());
        }
        return nullptr;
    }
    /**
     * @brief 检查是否存在组件
     * @tparam T 组件类型
     * @return 是否存在组件
     */
    template <typename T>
    bool hasComponent() const {
        static_assert(std::is_base_of<engine::component::Component, T>::value, "GAMEOBJECT::hasComponent::ERROR::T 必须继承自 Component");
        return m_components.contains(std::type_index(typeid(T))); // contains方法为 C++20 新增
    }
    /**
     * @brief 移除组件
     * @tparam T 组件类型
     */
    template <typename T>
    void removeComponent() {
        static_assert(std::is_base_of<engine::component::Component, T>::value, "GAMEOBJECT::removeComponent::ERROR::T 必须继承自 Component");
        auto typeIndex = std::type_index(typeid(T));
        auto it = m_components.find(typeIndex);
        if (it != m_components.end()) {
            it->second->clean();
            m_components.erase(it);
        }
    }
    /// @}

    /// @name 生命周期
    /// @{
    void handleInput(engine::core::Context &context);    /// @brief 处理输入
    void update(float deltaTime, engine::core::Context &context);    /// @brief 更新游戏对象
    void render(engine::core::Context &context);    /// @brief 渲染游戏对象
    void clean();    /// @brief 清理游戏对象
    /// @}
};

} // namespace engine::object
