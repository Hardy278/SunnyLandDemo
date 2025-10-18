#pragma once
#include <string>
#include <string_view>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <map>
#include <optional>
#include "../utils/Math.hpp"

namespace engine::component {
    class AnimationComponent;
    class AudioComponent;
    struct TileInfo;
    enum class TileType;
}

namespace engine::object {
    class ObjectBuilder;
}

namespace engine::core {
    class Context;
}

namespace engine::scene {
class Scene;

/**
 * @brief 负责从 Tiled JSON 文件 (.tmj) 加载关卡数据到 Scene 中。
 */
class LevelLoader final {
    friend class engine::object::ObjectBuilder;
private:
    std::string m_mapPath;      ///< @brief 地图路径（拼接路径时需要）
    glm::ivec2  m_mapSize;       ///< @brief 地图尺寸(瓦片数量)
    glm::ivec2  m_tileSize;      ///< @brief 瓦片尺寸(像素)
    std::map<int, nlohmann::json> m_tilesetData;    ///< @brief firstgid -> 瓦片集数据
    std::unique_ptr<engine::object::ObjectBuilder> m_objectBuilder;    ///< @brief 对象构建器

public:
    LevelLoader(engine::core::Context& context);
    ~LevelLoader();
    
    void setObjectBuilder(std::unique_ptr<engine::object::ObjectBuilder> objectBuilder);

    /**
     * @brief 加载关卡数据到指定的 Scene 对象中。
     * @param mapPath Tiled JSON 地图文件的路径。
     * @param scene 要加载数据的目标 Scene 对象。
     * @return bool 是否加载成功。
     */
    [[nodiscard]] bool loadLevel(std::string_view mapPath, Scene& scene);

private:
    void loadImageLayer(const nlohmann::json& layerJson, Scene& scene);    ///< @brief 加载图片图层
    void loadTileLayer(const nlohmann::json& layerJson, Scene& scene);     ///< @brief 加载瓦片图层
    void loadObjectLayer(const nlohmann::json& layerJson, Scene& scene);   ///< @brief 加载对象图层

    /**
     * @brief 添加动画到指定的 AnimationComponent。
     * @param animJson 动画json数据（自定义）
     * @param ac AnimationComponent 指针（动画添加到此组件）
     * @param spriteSize 每一帧动画的尺寸
     */
    void addAnimation(const nlohmann::json& animJson, engine::component::AnimationComponent* ac, const glm::vec2& spriteSize);

    /**
     * @brief 获取瓦片属性
     * @tparam T 属性类型
     * @param tileJson 瓦片json数据
     * @param propertyName 属性名称
     * @return 属性值，如果属性不存在则返回 std::nullopt
     */
    template<typename T>
    std::optional<T> getTileProperty(const nlohmann::json& tileJson, std::string_view propertyName) {
        if (!tileJson.contains("properties")) return std::nullopt;
        const auto& properties = tileJson["properties"];
        for (const auto& property : properties) {
            if (property.contains("name") && property["name"] == std::string(propertyName)) {
                if (property.contains("value")) {
                    return property["value"].get<T>();
                }
            }
        }
        return std::nullopt;
    }

    /**
     * @brief 获取瓦片碰撞器矩形
     * @param tileJson 瓦片json数据
     * @return 碰撞器矩形，如果碰撞器不存在则返回 std::nullopt
     */
    std::optional<engine::utils::Rect> getColliderRect(const nlohmann::json& tileJson);

    /**
     * @brief 根据瓦片json对象获取瓦片类型
     * @param tileJson 瓦片json数据
     * @return 瓦片类型
     */
    engine::component::TileType getTileType(const nlohmann::json& tileJson);

    /**
     * @brief 根据（单一图片）图块集中的id获取瓦片类型
     * @param tilesetJson 图块集json数据
     * @param localID 图块集中的id
     * @return 瓦片类型
     */
    engine::component::TileType getTileTypeByID(const nlohmann::json& tilesetJson, int localID);

    /**
     * @brief 根据全局 ID 获取瓦片信息。
     * @param gid 全局 ID。
     * @return engine::component::TileInfo 瓦片信息。
     */
    engine::component::TileInfo getTileInfoByGid(int gid);

    /**
     * @brief 根据全局 ID 获取瓦片json对象 (用于对象层获取瓦片信息)
     * @param gid 全局 ID
     * @return 瓦片json对象
     */
    std::optional<nlohmann::json> getTileJsonByGid(int gid) const;

    /**
     * @brief 加载 Tiled tileset 文件 (.tsj)。
     * @param tilesetPath Tileset 文件路径。
     * @param firstGid 此 tileset 的第一个全局 ID。
     */
    void loadTileset(std::string_view tilesetPath, int firstGid);

    /**
     * @brief 解析图片路径，合并地图路径和相对路径。例如：
     * @brief - 文件路径："assets/maps/level1.tmj"
     * @brief - 相对路径："../textures/Layers/back.png"
     * @brief - 最终路径："assets/textures/Layers/back.png"
     * @param relativePath 相对路径（相对于文件）
     * @param filePath 文件路径
     * @return std::string 解析后的完整路径。
     */
    std::string resolvePath(std::string_view relativePath, std::string_view filePath);
};

} // namespace engine::scene