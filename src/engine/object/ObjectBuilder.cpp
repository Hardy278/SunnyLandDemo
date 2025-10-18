#include "ObjectBuilder.hpp"
#include "GameObject.hpp"
#include "../core/Context.hpp"
#include "../component/ParallaxComponent.hpp"
#include "../component/TransformComponent.hpp"
#include "../component/TilelayerComponent.hpp"
#include "../component/SpriteComponent.hpp"
#include "../component/ColliderComponent.hpp"
#include "../component/PhysicsComponent.hpp"
#include "../component/AnimationComponent.hpp"
#include "../component/HealthComponent.hpp"
#include "../resource/ResourceManager.hpp"
#include "../physics/PhysicsEngine.hpp"
#include "../render/Animation.hpp"
#include "../scene/LevelLoader.hpp"

#include <spdlog/spdlog.h>

#include <string>
#include <string_view>

namespace engine::object {

ObjectBuilder::~ObjectBuilder() = default;

ObjectBuilder::ObjectBuilder(engine::scene::LevelLoader& level_loader, engine::core::Context& context)
    : m_levelLoader(level_loader), m_context(context) {
    spdlog::trace("OBJECTBUILDER::已创建");
}

ObjectBuilder *ObjectBuilder::configure(const nlohmann::json* objectJson) {
    reset();  // 重置生成器状态，确保每次配置都是干净的
    if (!objectJson) {
        spdlog::error("OBJECTBUILDER::configure::配置失败: objectJson 为空。");
        return this;    // 返回自身，允许链式调用
    }
    // 非矩形对象会有额外标识（目前不支持，直接返回）
    if (objectJson->value("point", false)) {             // 如果是点对象
        return this;
    } else if (objectJson->value("ellipse", false)) {    // 如果是椭圆对象
        return this;
    } else if (objectJson->value("polygon", false)) {    // 如果是多边形对象
        return this;
    } 
    // 没有这些标识则默认是矩形对象，可以继续
    m_objectJson = objectJson;
    m_tileJson = nullptr;         // 确保 m_tileJson 为空，作为自定义形状对象的标识
    spdlog::trace("OBJECTBUILDER::configure::配置成功: m_objectJson 已设置。");
    return this;
}

ObjectBuilder *ObjectBuilder::configure(const nlohmann::json *objectJson, const nlohmann::json *tileJson, engine::component::TileInfo tileInfo) {
    reset();
    if (!objectJson || !tileJson) {
        spdlog::error("OBJECTBUILDER::configure::配置失败: objectJson 或 tileJson 为空。");
        return this;
    }
    m_objectJson = objectJson;
    m_tileJson = tileJson;
    m_tileInfo = std::move(tileInfo);
    spdlog::trace("OBJECTBUILDER::configure::配置成功: m_objectJson 和 m_tileJson 已设置。");
    return this;
}

void ObjectBuilder::build() {
    if (!m_objectJson) {
        spdlog::error("OBJECTBUILDER::build::构建失败: m_objectJson 为空。");
        return;  // 如果没有配置，直接返回
    }
    // 按顺序构建各个组件
    buildBase();          // 构建基本信息
    buildTransform();     // 构建变换组件
    buildSprite();        // 构建精灵组件（如果是图片对象）
    buildPhysics();       // 构建物理组件
    buildAnimation();     // 构建动画组件（如果有）
    buildHealth();        // 构建生命值组件（如果有）
    spdlog::debug("OBJECTBUILDER::build::构建完成: {}", m_name);
}

std::unique_ptr<GameObject> ObjectBuilder::getGameObject() {
    return std::move(m_gameObject);  // 返回构建好的 GameObject，并将其所有权转移出去
}

void ObjectBuilder::reset() {
    m_objectJson = nullptr;  // 重置为 nullptr
    m_tileJson = nullptr;    // 重置为 nullptr
    m_tileInfo = {};         // 重置 TileInfo
    m_gameObject = nullptr;  // 重置 GameObject
    m_name.clear();           // 清空名称
    m_dstSize = glm::vec2(0.0f);  // 重置目标尺寸
    m_srcSize = glm::vec2(0.0f);  // 重置源尺寸
}

void ObjectBuilder::buildBase() {
    // 名称信息从 m_objectJson 中获取
    m_name = m_objectJson->value("name", "");
    
    // 先查找地图中的标签，没有再查找图块集中的标签（前者覆盖后者）
    auto tag = getTileProperty<std::string>(*m_objectJson, "tag");

    if (!tag && m_tileJson) {   // 如果没找到标签且 m_tileJson 存在，则尝试从中获取标签
        tag = getTileProperty<std::string>(*m_tileJson, "tag");
        // 如果也没找到标签，但它是危险图块，则自动设置标签为 "hazard"
        if (!tag && m_tileInfo.type == engine::component::TileType::HAZARD) {
            tag = "hazard";  // 危险图块默认标签
        }
    }
    // 创建游戏对象
    m_gameObject = std::make_unique<GameObject>(m_name, tag.value_or(""));
}

void ObjectBuilder::buildTransform() {
    // 位置、尺寸和旋转信息从 m_objectJson 中获取
    auto position = glm::vec2(m_objectJson->value("x", 0.0f), m_objectJson->value("y", 0.0f));
    m_dstSize = glm::vec2(m_objectJson->value("width", 0.0f), m_objectJson->value("height", 0.0f));
    auto rotation = m_objectJson->value("rotation", 0.0f);
    auto scale = glm::vec2(1.0f);  // 默认缩放为1.0f
    // 如果是图片对象，需要额外处理瓦片json中的数据
    if (m_tileJson) {
        position = glm::vec2(position.x, position.y - m_dstSize.y);  // 图片对象的position需要进行调整(左下角到左上角)
        // --- 计算缩放比例 ---
        auto m_srcSizeopt = m_tileInfo.sprite.getSourceRect();
        if (m_srcSizeopt) {        // 正常情况下，所有瓦片的Sprite都设置了源矩形，没有就跳过
            m_srcSize = glm::vec2(m_srcSizeopt->w, m_srcSizeopt->h);
            scale = m_dstSize / m_srcSize;    // 更新缩放比例
        }   
    }
    // 添加 TransformComponent
    m_gameObject->addComponent<engine::component::TransformComponent>(position, scale, rotation);
}

void ObjectBuilder::buildSprite() {
    // 如果是自定义形状对象，则不需要SpriteComponent
    if (!m_tileJson) return; 
    // 确保 m_tileInfo 已经被正确配置
    if (m_tileInfo.sprite.getTextureID().empty()) {
        spdlog::error("OBJECTBUILDER::buildSprite::对象 '{}' 在 m_tileInfo 中的 sprite 没有纹理 ID。", m_name);
        return;  // 如果没有纹理ID，则不添加 SpriteComponent
    }
    // 添加 SpriteComponent
    m_gameObject->addComponent<engine::component::SpriteComponent>(m_tileInfo.sprite, m_context.getResourceManager());
}

void ObjectBuilder::buildPhysics() {
    // 如果是自定义形状对象
    if (!m_tileJson){
        // 碰撞盒大小与dst_size相同 
        auto collider = std::make_unique<engine::physics::AABBCollider>(m_dstSize);
        auto* cc = m_gameObject->addComponent<engine::component::ColliderComponent>(std::move(collider));
        // 自定义形状通常是trigger类型，除非显式指定 （因此默认为真）
        cc->setTrigger(m_objectJson->value("trigger", true));
        // 添加物理组件，不受重力影响
        m_gameObject->addComponent<engine::component::PhysicsComponent>(&m_context.getPhysicsEngine(), false);
    }
    // 如果是图片对象
    else {
        // 获取碰信息: 如果是SOLID类型，则添加物理组件，且图片源矩形区域就是碰撞盒大小
        if (m_tileInfo.type == engine::component::TileType::SOLID) {
            auto collider = std::make_unique<engine::physics::AABBCollider>(m_srcSize);
            m_gameObject->addComponent<engine::component::ColliderComponent>(std::move(collider));
            m_gameObject->addComponent<engine::component::PhysicsComponent>(&m_context.getPhysicsEngine(), false);  // 物理组件不受重力影响
            m_gameObject->setTag("solid");  // 设置标签方便物理引擎检索
        }
        // 如果非SOLID类型，检查自定义碰撞盒是否存在
        else if (auto rect = getColliderRect(*m_tileJson); rect) {  
            // 如果有，添加碰撞组件
            auto collider = std::make_unique<engine::physics::AABBCollider>(rect->size);
            auto* cc = m_gameObject->addComponent<engine::component::ColliderComponent>(std::move(collider));
            cc->setOffset(rect->position);  // 自定义碰撞盒的坐标是相对于图片坐标，也就是针对Transform的偏移量
            m_gameObject->addComponent<engine::component::PhysicsComponent>(&m_context.getPhysicsEngine(), false);    // 和物理组件（默认不受重力影响）
        }
        // 尝试获取显式标注的重力信息并设置
        auto gravity = getTileProperty<bool>(*m_tileJson, "gravity");
        if (gravity) {
            auto pc = m_gameObject->getComponent<engine::component::PhysicsComponent>();
            if (pc) {
                pc->setUseGravity(gravity.value());
            } else {
                spdlog::warn("OBJECTBUILDER::buildPhysics::对象 '{}' 在设置重力信息时没有物理组件，请检查地图设置。", m_name);
                m_gameObject->addComponent<engine::component::PhysicsComponent>(&m_context.getPhysicsEngine(), gravity.value());
            }
        }
    }
}

void ObjectBuilder::buildAnimation()
{
    if (!m_tileJson) return;  // 如果是自定义形状对象，则不需要 AnimationComponent

    auto animString = getTileProperty<std::string>(*m_tileJson, "animation");
    if (animString) {
        // 解析string为JSON对象
        nlohmann::json animJson;
        try {
            animJson = nlohmann::json::parse(animString.value());
        } catch (const nlohmann::json::parse_error& e) {
            spdlog::error("OBJECTBUILDER::buildAnimation::解析动画 JSON 字符串失败: {}", e.what());
            return;  // 跳过此对象
        }
        // 添加AnimationComponent
        auto* ac = m_gameObject->addComponent<engine::component::AnimationComponent>();
        // 添加动画到 AnimationComponent
        addAnimation(animJson, ac, m_srcSize);
    }
}

void ObjectBuilder::buildHealth()
{
    if (!m_tileJson) return;  // 如果是自定义形状对象，则不需要 HealthComponent

    // 获取生命值信息并设置
    auto health = getTileProperty<int>(*m_tileJson, "health");
    if (health) {
        // 添加 HealthComponent
        m_gameObject->addComponent<engine::component::HealthComponent>(health.value());
    }
}

// --- 代理函数，让子类能获取到LevelLoader的私有方法 ---
template<typename T>
std::optional<T> ObjectBuilder::getTileProperty(const nlohmann::json& tileJson, std::string_view propertyName) {
    return m_levelLoader.getTileProperty<T>(tileJson, propertyName);
}

engine::component::TileType ObjectBuilder::getTileType(const nlohmann::json& tileJson) {
    return m_levelLoader.getTileType(tileJson);
}

std::optional<engine::utils::Rect> ObjectBuilder::getColliderRect(const nlohmann::json& tileJson) {
    return m_levelLoader.getColliderRect(tileJson);
}

void ObjectBuilder::addAnimation(const nlohmann::json& animJson, engine::component::AnimationComponent* ac, const glm::vec2& spriteSize) {
    m_levelLoader.addAnimation(animJson, ac, spriteSize);
}

} // namespace engine::object