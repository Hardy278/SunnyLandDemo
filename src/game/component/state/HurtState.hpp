#pragma once
#include "PlayerState.hpp"

namespace game::component::state {

class HurtState final : public PlayerState {
    friend class game::component::PlayerComponent;
private:
    float m_stunnedTimer = 0.0f;  ///< @brief 硬直计时器，单位为秒
    
public:
    HurtState(PlayerComponent* playerComponent) : PlayerState(playerComponent) {}
    ~HurtState() override = default;

private:
    void enter() override;
    void exit() override;
    std::unique_ptr<PlayerState> update(float deltaTime, engine::core::Context&) override;
};

} // namespace game::component::state