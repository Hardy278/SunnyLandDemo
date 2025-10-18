#include "ObjectBuilderSL.hpp"
#include "../../engine/object/GameObject.hpp"
#include "../../engine/core/Context.hpp"
#include "../../engine/render/Camera.hpp"
#include "../../engine/render/Animation.hpp"
#include "../../engine/component/TransformComponent.hpp"
#include "../../engine/component/AnimationComponent.hpp"
#include "../component/AIComponent.hpp"
#include "../component/ai/PatrolBehavior.hpp"
#include "../component/ai/UpdownBehavior.hpp"
#include "../component/ai/JumpBehavior.hpp"
#include "../component/PlayerComponent.hpp"
#include <spdlog/spdlog.h>

namespace game::object {

ObjectBuilderSL::ObjectBuilderSL(engine::scene::LevelLoader &levelLoader, engine::core::Context &context) 
    : engine::object::ObjectBuilder(levelLoader, context) {
    spdlog::trace("OBJECTBUILDERSL::构造完成");
}

void ObjectBuilderSL::build() {
    engine::object::ObjectBuilder::build();
    buildPlayer();
    buildEnemy();
    buildItem();
    spdlog::trace("OBJECTBUILDERSL::build::完成生成");
}

void ObjectBuilderSL::buildPlayer() {
    spdlog::trace("OBJECTBUILDERSL::buildPlayer::开始构建玩家");
    if (!m_tileJson || m_gameObject->getTag() != "player") return;
    if (auto* playerComponent = m_gameObject->addComponent<game::component::PlayerComponent>(); playerComponent) {
        if (auto* transform = m_gameObject->getComponent<engine::component::TransformComponent>(); transform) {
            m_context.getCamera().setTarget(transform);
        }
    }
}

void ObjectBuilderSL::buildEnemy() {
    spdlog::trace("OBJECTBUILDERSL::buildEnemy::开始构建敌人");
    if (!m_tileJson || m_gameObject->getTag() != "enemy") return;
    if (m_name == "eagle") {
        if (auto* AIComponent = m_gameObject->addComponent<game::component::AIComponent>(); AIComponent){
            auto yMax = m_gameObject->getComponent<engine::component::TransformComponent>()->getPosition().y;
            auto yMin = yMax - 80.0f;    // 让鹰的飞行范围 (当前位置与上方80像素 的区域)
            AIComponent->setBehavior(std::make_unique<game::component::ai::UpDownBehavior>(yMin, yMax));
        }
    }
    if (m_name == "frog") {
        if (auto* AIComponent = m_gameObject->addComponent<game::component::AIComponent>(); AIComponent){
            auto xMax = m_gameObject->getComponent<engine::component::TransformComponent>()->getPosition().x - 10.0f;
            auto xMin = xMax - 90.0f;    // 青蛙跳跃范围（右侧 - 10.0f 是为了增加稳定性）
            AIComponent->setBehavior(std::make_unique<game::component::ai::JumpBehavior>(xMin, xMax));
        }
    }
    if (m_name == "opossum") {
        if (auto* AIComponent = m_gameObject->addComponent<game::component::AIComponent>(); AIComponent){
            auto xMax = m_gameObject->getComponent<engine::component::TransformComponent>()->getPosition().x;
            auto xMin = xMax - 200.0f;    // 负鼠巡逻范围
            AIComponent->setBehavior(std::make_unique<game::component::ai::PatrolBehavior>(xMin, xMax));
        }
    }
}

void ObjectBuilderSL::buildItem() {
    spdlog::trace("OBJECTBUILDERSL::buildItem::构建物品");
    if (!m_tileJson || m_gameObject->getTag() != "item") return;
    if (auto* ac = m_gameObject->getComponent<engine::component::AnimationComponent>(); ac){
        ac->playAnimation("idle");
    }
}

} // namespace game::object