#include "Context.hpp"
#include "../input/InputManager.hpp"
#include "../render/Renderer.hpp"
#include "../render/Camera.hpp"
#include "../resource/ResourceManager.hpp"
#include "../physics/PhysicsEngine.hpp"

#include <spdlog/spdlog.h>

namespace engine::core {
Context::Context(
    engine::input::InputManager &inputManager, 
    engine::render::Renderer &renderer, 
    engine::render::Camera &camera, 
    engine::resource::ResourceManager &resourceManager, 
    engine::physics::PhysicsEngine &physicsEngine
) : m_inputManager(inputManager), m_renderer(renderer), m_camera(camera), m_resourceManager(resourceManager), m_physicsEngine(physicsEngine) {
    spdlog::trace("CONTEXT::上下文已创建，包括：输入管理器、渲染器、相机、资源管理器和物理引擎");
}

/// @name getter
/// @{
engine::input::InputManager &Context::getInputManager() const {
    return m_inputManager;
}

engine::render::Renderer &Context::getRenderer() const {
    return m_renderer;
}

engine::render::Camera &Context::getCamera() const {
    return m_camera;
}

engine::resource::ResourceManager &Context::getResourceManager() const {
    return m_resourceManager;
}
engine::physics::PhysicsEngine &Context::getPhysicsEngine() const {
    return m_physicsEngine;
}
/// @}

} // namespace engine::core