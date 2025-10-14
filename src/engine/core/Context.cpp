#include "Context.hpp"
#include "../input/InputManager.hpp"
#include "../render/Renderer.hpp"
#include "../render/Camera.hpp"
#include "../render/TextRenderer.hpp"
#include "../resource/ResourceManager.hpp"
#include "../physics/PhysicsEngine.hpp"

#include <spdlog/spdlog.h>

namespace engine::core {
Context::Context(
    engine::input::InputManager &inputManager, 
    engine::render::Renderer &renderer, 
    engine::render::Camera &camera, 
    engine::render::TextRenderer &textRenderer,
    engine::resource::ResourceManager &resourceManager, 
    engine::physics::PhysicsEngine &physicsEngine,
    engine::core::GameState& gameState
) : m_inputManager(inputManager), m_renderer(renderer), m_camera(camera),
    m_textRenderer(textRenderer), m_resourceManager(resourceManager), m_physicsEngine(physicsEngine), m_gameState(gameState) {
    spdlog::trace("CONTEXT::上下文已创建，包括：输入管理器、渲染器、相机、资源管理器、物理引擎和游戏状态");
}
} // namespace engine::core