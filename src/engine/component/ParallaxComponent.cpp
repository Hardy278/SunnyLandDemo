#include "ParallaxComponent.hpp"
#include "TransformComponent.hpp"
#include "../object/GameObject.hpp"
#include "../core/Context.hpp"
#include "../render/Renderer.hpp"

#include <spdlog/spdlog.h>

namespace engine::component {
ParallaxComponent::ParallaxComponent(std::string_view textureID, glm::vec2 scrollFactor, glm::bvec2 repeat) 
    : m_sprite(engine::render::Sprite(textureID)), m_scrollFactor(scrollFactor), m_repeat(repeat) {
    spdlog::trace("PARALLAXCOMPONENT::ParallaxComponent初始化完成, 纹理ID:{}", textureID);
}

void ParallaxComponent::init() {
    if (!m_owner) {
        spdlog::error("PARALLAXCOMPONENT::init::初始化失败, 没有拥有者");
        return;
    }
    m_transform = m_owner->getComponent<TransformComponent>();
    if (!m_transform) {
        spdlog::error("PARALLAXCOMPONENT::init::初始化失败, GameObject 没有找到 TransformComponent 组件");
        return;
    }
}

void ParallaxComponent::render(engine::core::Context &context) {
    if (m_isHidden || !m_owner) {
        return;
    }
    context.getRenderer().drawParallax(context.getCamera(), m_sprite, m_transform->getPosition(), m_scrollFactor, m_repeat);
}

} // namespace engine::component