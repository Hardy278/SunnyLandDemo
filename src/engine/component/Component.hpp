#pragma once
#include "../core/Context.hpp"

namespace engine::object {
class GameObject;
} // namespace engine::object

namespace engine::core {
class Context;
}

namespace engine::component {

/**
 * @brief 组件基类，所有游戏组件的父类
 * 
 * Component是所有游戏组件的基类，提供了基本的组件功能接口
 * 
 * @note 组件需要附加到GameObject上才能发挥作用
 * @note 每个组件都拥有一个指向其拥有者 GameObject 的指针，通过这个指针可以访问GameObject的属性和方法
 * @note 组件系统采用组合模式，允许通过组合不同的组件来创建复杂的游戏对象
 */
class Component {
    friend class engine::object::GameObject;
    
protected:
    engine::object::GameObject* m_owner = nullptr; ///< @brief 指向拥有此组件的游戏对象的指针

public:
    Component() = default;
    virtual ~Component() = default;

    
    /// @name 禁止拷贝构造和赋值操作
    /// @{
    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;
    Component(Component&&) = delete;
    Component& operator=(Component&&) = delete;
    /// @}


    /// @name setter / getter
    /// @{
    /**
     * @brief 设置组件的拥有者
     * @param owner 指向拥有此组件的游戏对象的指针
     * 
     * 此方法由GameObject在添加组件时自动调用，不应该手动调用。
     * 设置拥有者后，组件可以通过m_owner访问GameObject的属性和方法。
     */
    void setOwner(engine::object::GameObject* owner);
    /**
     * @brief 获取组件的拥有者
     * @return 指向拥有此组件的游戏对象的指针
     * 
     * 通过这个指针可以访问GameObject的属性和方法。
     * 如果组件尚未被添加到GameObject，返回nullptr。
     */
    engine::object::GameObject* getOwner() const;
    /// @}

protected:
    /// @name 组件生命周期方法
    /// @{
    virtual void init() {}
    virtual void handleInput(engine::core::Context&) {}
    virtual void update(float, engine::core::Context&) {}
    virtual void render(engine::core::Context&) {}
    virtual void clean() {}
    /// @}
};
} // namespace engine::component
