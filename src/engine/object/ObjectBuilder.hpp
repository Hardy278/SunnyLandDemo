#pragma once
#include "../component/TilelayerComponent.hpp"
#include "../utils/Math.hpp"

#include <nlohmann/json_fwd.hpp>
#include <glm/vec2.hpp>

#include <optional>

namespace engine::scene {
    class LevelLoader;
}

namespace engine::component {
    class AnimationComponent;
}

namespace engine::core {
    class Context;
}

namespace engine::object {
class GameObject;

class ObjectBuilder {
protected:
    engine::scene::LevelLoader& m_levelLoader;
    engine::core::Context&      m_context;
    std::unique_ptr<GameObject> m_gameObject;

    // 解析游戏对象需要的关键信息
    const nlohmann::json*        m_objectJson = nullptr;
    const nlohmann::json*        m_tileJson   = nullptr;
    engine::component::TileInfo  m_tileInfo;

    // 解析游戏对象需要多次运用的数据
    std::string m_name;
    glm::vec2   m_dstSize;
    glm::vec2   m_srcSize;

public:
    explicit ObjectBuilder(engine::scene::LevelLoader& levelLoader, engine::core::Context& context);
    virtual ~ObjectBuilder();

    /// @name 三个关键方法：配置、构建、返回
    /// @{
    ObjectBuilder* configure(const nlohmann::json* objectJson);   /// @brief 配置对象，针对自定义对象
    ObjectBuilder* configure(const nlohmann::json* objectJson, const nlohmann::json* tileJson, engine::component::TileInfo tileInfo); /// @brief 配置对象，针对图片对象
    virtual void build(); /// @brief 构建对象
    /// @}

    std::unique_ptr<GameObject> getGameObject();

protected:
    void reset(); /// @brief 重置对象构建器

    /// @name 代理函数
    /// @{
    template<typename T>
    std::optional<T> getTileProperty(const nlohmann::json& json, const std::string_view propertyName);
    engine::component::TileType getTileType(const nlohmann::json& tileJson);
    std::optional<engine::utils::Rect> getColliderRect(const nlohmann::json& tileJson);
    void addAnimation(const nlohmann::json& animJson, engine::component::AnimationComponent* ac, const glm::vec2& spriteSize);
    /// @}

    /// @name 解析游戏对象需要的关键信息
    /// @{
    void buildBase();
    void buildTransform();
    void buildSprite();
    void buildPhysics();
    void buildAnimation();
    void buildHealth();
    /// @}
};


} // namespace engine::object