#include "GameObject.hpp"
#include "../render/Renderer.hpp"
#include "../render/Camera.hpp"
#include "../input/InputManager.hpp"

namespace engine::object {

GameObject::GameObject(std::string_view name, std::string_view tag): m_name(name), m_tag(tag) {
    spdlog::info("GAMEOBJECT::GameObject 初始化成功: {} {}", m_name, m_tag);
}

/// @name 生命周期
/// @{
void GameObject::update(float deltaTime, engine::core::Context &context) {
    // 遍历所有组件并调用它们的 update 方法
    for (auto& pair : m_components) {
        pair.second->update(deltaTime, context);
    }
}

void GameObject::render(engine::core::Context &context) {
    // 遍历所有组件并调用它们的 render 方法
    for (auto& pair : m_components) {
        pair.second->render(context);
    }
}

void GameObject::clean() {
    spdlog::trace("GAMEOBJECT::clean::清空 GameObject... {} {}", m_name, m_tag);
    // 遍历所有组件并调用它们的 clean 方法
    for (auto& pair : m_components) {
        pair.second->clean();
    }
    m_components.clear(); // 清空 map, unique_ptr 会自动释放内存
}

void GameObject::handleInput(engine::core::Context &context) {
    // 遍历所有组件并调用它们的 handleInput 方法
    for (auto& pair : m_components) {
        pair.second->handleInput(context);
    }
}
/// @}
} // namespace engine::object