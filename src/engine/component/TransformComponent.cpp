#include "TransformComponent.hpp"
#include "SpriteComponent.hpp"
#include "ColliderComponent.hpp"
#include "../object/GameObject.hpp"

namespace engine::component { 


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