#include "TransformComponent.hpp"
#include "SpriteComponent.hpp"
#include "ColliderComponent.hpp"
#include "../object/GameObject.hpp"

namespace engine::component { 

const glm::vec2 &TransformComponent::getPosition() const {
    return m_position;
}
float TransformComponent::getRotation() const {
    return m_rotation;
}
const glm::vec2 &TransformComponent::getScale() const {
    return m_scale;
}
void TransformComponent::setPosition(glm::vec2 position) {
    m_position = std::move(position);
}
void TransformComponent::setRotation(float rotation) {
    m_rotation = rotation;
}
void TransformComponent::setScale(glm::vec2 scale) {
    m_scale = std::move(scale);
    if (m_owner) {
        auto spriteComp = m_owner->getComponent<SpriteComponent>();
        if (spriteComp) {
            spriteComp->updateOffset();
        }
        auto colliderComp = m_owner->getComponent<ColliderComponent>();
        if (colliderComp) {
            colliderComp->updateOffset();
        }
    }
}
void TransformComponent::translate(const glm::vec2 &offset) {
    m_position += offset;
}

} // namespace engine::component