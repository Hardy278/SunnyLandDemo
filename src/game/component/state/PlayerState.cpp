#include "PlayerState.hpp"
#include "../PlayerComponent.hpp"
#include "../../../engine/component/AnimationComponent.hpp"
#include "../../../engine/object/GameObject.hpp"
#include <spdlog/spdlog.h>

namespace game::component::state {

void PlayerState::playAnimation(const std::string& animationName) {
    if (!m_playerComponent) {
        spdlog::error("PLAYERSTATE::playAnimation::ERROR::PlayerState 没有关联的 PlayerComponent，无法播放动画 '{}'", animationName);
        return;
    }
    auto animationComponent = m_playerComponent->getAnimationComponent();
    if (!animationComponent) {
        spdlog::error("PLAYERSTATE::playAnimation::ERROR::PlayerComponent '{}' 没有 AnimationComponent，无法播放动画 '{}'", m_playerComponent->getOwner()->getName(), animationName);
        return;
    }
    animationComponent->playAnimation(animationName);
}

} // namespace game::component::state