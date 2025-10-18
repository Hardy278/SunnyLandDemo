#pragma once
#include "PlayerState.hpp"

namespace game::component::state {

class ClimbState final : public PlayerState {
    friend class game::component::PlayerComponent;

    bool m_isMoving = false;    ///< @brief 记录是否正在移动，用于判断是否需要切换状态
public:
    ClimbState(PlayerComponent* playerComponent) : PlayerState(playerComponent) {}
    ~ClimbState() override = default;

private:
    void enter() override;
    void exit() override;
    std::unique_ptr<PlayerState> update(float deltaTime, engine::core::Context&) override;

    std::unique_ptr<PlayerState> moveLeft() override;
    std::unique_ptr<PlayerState> moveRight() override;
    std::unique_ptr<PlayerState> jump() override;
    std::unique_ptr<PlayerState> climbUp() override;
    std::unique_ptr<PlayerState> climbDown() override;
};

} // namespace game::component::state