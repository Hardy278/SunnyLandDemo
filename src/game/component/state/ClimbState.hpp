#pragma once
#include "PlayerState.hpp"

namespace game::component::state {

class ClimbState final : public PlayerState {
    friend class game::component::PlayerComponent;
public:
    ClimbState(PlayerComponent* playerComponent) : PlayerState(playerComponent) {}
    ~ClimbState() override = default;

private:
    void enter() override;
    void exit() override;
    std::unique_ptr<PlayerState> handleInput(engine::core::Context&) override;
    std::unique_ptr<PlayerState> update(float deltaTime, engine::core::Context&) override;
};

} // namespace game::component::state