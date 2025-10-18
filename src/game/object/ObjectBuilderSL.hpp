#pragma once
#include "../../engine/object/ObjectBuilder.hpp"

namespace game::object {

class ObjectBuilderSL : public engine::object::ObjectBuilder {
public:
    ObjectBuilderSL(engine::scene::LevelLoader& levelLoader, engine::core::Context& context);
    ~ObjectBuilderSL() = default;

    void build() override;
private:
    void buildPlayer();
    void buildEnemy();
    void buildItem();
};

} // namespace game::object