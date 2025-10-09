#include "DeadState.hpp"
#include "../PlayerComponent.hpp"
#include "../../../engine/object/GameObject.hpp"
#include "../../../engine/component/AnimationComponent.hpp"
#include "../../../engine/component/PhysicsComponent.hpp"
#include "../../../engine/component/ColliderComponent.hpp"

namespace game::component::state {

void DeadState::enter() {
    spdlog::debug("DEADSTATE::enter::DEBUG::玩家进入死亡状态。");
    playAnimation("hurt");  // 播放死亡(受伤)动画

    // 应用击退力（只向上）
    auto physicsComponent = m_playerComponent->getPhysicsComponent();
    physicsComponent->m_velocity = glm::vec2(0.0f, -200.0f);  // 向上击退

    // 禁用碰撞(自动掉出屏幕)
    auto colliderComponent = m_playerComponent->getOwner()->getComponent<engine::component::ColliderComponent>();
    if (colliderComponent) {
        colliderComponent->setActive(false);
    }
}

void DeadState::exit() {
}

std::unique_ptr<PlayerState> DeadState::handleInput(engine::core::Context&) {
    // 死亡状态下不处理输入
    return nullptr;
}

std::unique_ptr<PlayerState> DeadState::update(float, engine::core::Context&) {
    // 死亡状态下不更新状态
    return nullptr;
}

}