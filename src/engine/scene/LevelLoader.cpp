#include "LevelLoader.hpp"
#include "../object/GameObject.hpp"
#include "../render/Animation.hpp"
#include "../component/TransformComponent.hpp"
#include "../component/HealthComponent.hpp"
#include "../component/SpriteComponent.hpp"
#include "../component/AnimationComponent.hpp"
#include "../component/ParallaxComponent.hpp"
#include "../component/ColliderComponent.hpp"
#include "../component/PhysicsComponent.hpp"
#include "../component/TilelayerComponent.hpp"
#include "../scene/Scene.hpp"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <glm/vec2.hpp>

#include <fstream>
#include <filesystem>

namespace engine::scene {
bool LevelLoader::loadLevel(std::string_view levelPath, Scene& scene) {
    // 1. 加载 JSON 文件
    auto path = std::filesystem::path(levelPath);
    std::ifstream file(path);
    if (!file.is_open()) {
        spdlog::error("LEVELLOADER::loadLevel::ERROR::无法打开关卡文件: {}", levelPath);
        return false;
    }
    // 2. 解析 JSON 数据
    nlohmann::json jsonData;
    try {
        file >> jsonData;
    } catch (const nlohmann::json::parse_error& e) {
        spdlog::error("LEVELLOADER::loadLevel::ERROR::解析 JSON 数据失败: {}", e.what());
        return false;
    }
    // 3. 获取基本地图信息 (名称、地图尺寸、瓦片尺寸)
    m_mapPath = levelPath;
    m_mapSize = glm::ivec2(jsonData.value("width", 0), jsonData.value("height", 0));
    m_tileSize = glm::ivec2(jsonData.value("tilewidth", 0), jsonData.value("tileheight", 0));
    // 4. 加载 tileset 数据
    if (jsonData.contains("tilesets") && jsonData["tilesets"].is_array()) {
        for (const auto& tilesetJson : jsonData["tilesets"]) {
            if (!tilesetJson.contains("source") || !tilesetJson["source"].is_string() ||
                !tilesetJson.contains("firstgid") || !tilesetJson["firstgid"].is_number_integer()) {
                spdlog::error("LEVELLOADER::loadLevel::ERROR::tilesets 对象中缺少有效 'source' 或 'firstgid' 字段。");
                continue;
            }
            auto tilesetPath = resolvePath(tilesetJson["source"].get<std::string>(), m_mapPath);  // 支持隐式转换，可以省略.get<T>()方法，
            auto firstGid = tilesetJson["firstgid"];
            loadTileset(tilesetPath, firstGid);
        }
    }
    // 5. 加载图层数据
    if (!jsonData.contains("layers") || !jsonData["layers"].is_array()) {       // 地图文件中必须有 layers 数组
        spdlog::error("LEVELLOADER::loadLevel::ERROR::地图文件 '{}' 中缺少或无效的 'layers' 数组。", levelPath);
        return false;
    }
    for (const auto& layerJson : jsonData["layers"]) {
        // 获取各图层对象中的类型（type）字段
        std::string layerType = layerJson.value("type", "none");
        if (!layerJson.value("visible", true)) {
            spdlog::info("LEVELLOADER::loadLevel::INFO::图层 '{}' 不可见，跳过加载。", layerJson.value("name", "Unnamed"));
            continue;
        }
        // 根据图层类型决定加载方法
        if (layerType == "imagelayer") {       
            loadImageLayer(layerJson, scene);
        } else if (layerType == "tilelayer") {
            loadTileLayer(layerJson, scene);
        } else if (layerType == "objectgroup") {
            loadObjectLayer(layerJson, scene);
        } else {
            spdlog::warn("LEVELLOADER::loadLevel::WARN::不支持的图层类型: {}", layerType);
        }
    }

    spdlog::info("LEVELLOADER::loadLevel::INFO::关卡加载完成: {}", levelPath);
    return true;
}

void LevelLoader::loadImageLayer(const nlohmann::json& layerJson, Scene& scene) {
    // 获取纹理相对路径 （会自动处理'\/'符号）
    // json.value()返回的是一个临时对象，需要赋值才能保存，不能直接使用std::string_view
    std::string imagePath = layerJson.value("image", "");
    if (imagePath.empty()) {
        spdlog::error("LEVELLOADER::loadImageLayer::ERROR::图层 '{}' 缺少 'image' 属性。", layerJson.value("name", "Unnamed"));
        return;
    }
    auto textureID = resolvePath(imagePath, m_mapPath);
    // 获取图层偏移量（json中没有则代表未设置，给默认值即可）
    const glm::vec2 offset = glm::vec2(layerJson.value("offsetx", 0.0f), layerJson.value("offsety", 0.0f));
    // 获取视差因子及重复标志
    const glm::vec2 scrollFactor = glm::vec2(layerJson.value("parallaxx", 1.0f), layerJson.value("parallaxy", 1.0f));
    const glm::bvec2 repeat = glm::bvec2(layerJson.value("repeatx", false), layerJson.value("repeaty", false));
    // 获取图层名称
    std::string layerName = layerJson.value("name", "Unnamed");
    
    /*  可用类似方法获取其它各种属性，这里我们暂时用不上 */
    
    // 创建游戏对象
    auto gameObject = std::make_unique<engine::object::GameObject>(layerName);
    // 依次添加Transform，Parallax组件
    gameObject->addComponent<engine::component::TransformComponent>(offset);
    gameObject->addComponent<engine::component::ParallaxComponent>(textureID, scrollFactor, repeat);
    // 添加到场景中
    scene.addGameObject(std::move(gameObject));
    spdlog::info("LEVELLOADER::loadImageLayer::INFO::加载图层: '{}' 完成", layerName);
}

void LevelLoader::loadTileLayer(const nlohmann::json& layerJson, Scene& scene) {
    if (!layerJson.contains("data") || !layerJson["data"].is_array()) {
        spdlog::error("LEVELLOADER::loadTileLayer::ERROR::图层 '{}' 缺少 'data' 属性", layerJson.value("name", "Unnamed"));
        return;
    }
    // 准备 TileInfo Vector (瓦片数量 = 地图宽度 * 地图高度)
    std::vector<engine::component::TileInfo> tiles;
    tiles.reserve(m_mapSize.x * m_mapSize.y);

    // 获取图层数据 (瓦片 ID 列表)
    const auto& data = layerJson["data"];

    // 根据gid获取必要信息，并依次填充 TileInfo Vector
    for (const auto& gid : data) {
        tiles.push_back(getTileInfoByGid(gid));
    }

    // 获取图层名称
    std::string layerName = layerJson.value("name", "Unnamed");
    // 创建游戏对象
    auto gameObject = std::make_unique<engine::object::GameObject>(layerName);
    // 添加Tilelayer组件
    gameObject->addComponent<engine::component::TileLayerComponent>(m_tileSize, m_mapSize, std::move(tiles));
    // 添加到场景中
    scene.addGameObject(std::move(gameObject));
    spdlog::info("LEVELLOADER::loadTileLayer::加载瓦片图层: '{}' 完成", layerName);
}
void LevelLoader::loadObjectLayer(const nlohmann::json &layerJson, Scene &scene) {
    if (!layerJson.contains("objects") || !layerJson["objects"].is_array()) {
        spdlog::error("LEVELLOADER::loadObjectLayer::ERROR::图层 '{}' 缺少 'objects' 属性", layerJson.value("name", "Unnamed"));
        return;
    }
    // 获取对象数据
    const auto& objects = layerJson["objects"];
    // 遍历对象数据
    for (const auto& object : objects) {
        auto gid = object.value("gid", 0);
        if (gid == 0) { // 如果 gid 为 0，则代表自己绘制的形状（可能是碰撞盒、触发器等，未来按需处理）
            // TODO: 处理自定义形状对象
        } else {
            // 根据gid获取必要信息
            auto tileInfo = getTileInfoByGid(gid);
            if (tileInfo.sprite.getTextureID().empty()) {
                spdlog::error("LEVELLOADER::loadObjectLayer::ERROR::gid 为 {} 的对象缺少纹理", gid);
                continue;
            }
            auto position = glm::vec2(object.value("x", 0.0f), object.value("y", 0.0f));
            auto dstSize = glm::vec2(object.value("width", 0.0f), object.value("height", 0.0f));
            position = glm::vec2(position.x, position.y - dstSize.y); // Tiled 的坐标原点在左下角，而引擎的坐标原点在左上角，所以需要转换

            auto rotation = object.value("rotation", 0.0f);
            auto srcSizeOpt = tileInfo.sprite.getSourceRect();
            if (!srcSizeOpt) {
                spdlog::error("LEVELLOADER::loadObjectLayer::ERROR::gid 为 {} 的对象缺少源尺寸", gid);
                continue;
            }
            auto srcSize = glm::vec2(srcSizeOpt->w, srcSizeOpt->h);    // 成员变量除了 value().w 外，也可以这样获取
            auto scale = dstSize / srcSize;
            // 获取对象名称
            std::string objectName = object.value("name", "Unnamed");

            // 创建游戏对象并添加组件
            auto gameObject = std::make_unique<object::GameObject>(objectName);
            gameObject->addComponent<component::TransformComponent>(position, scale, rotation);
            gameObject->addComponent<component::SpriteComponent>(std::move(tileInfo.sprite), scene.getContext().getResourceManager());

            // 获取瓦片json信息      
            // 1. 必然存在，因为getTileInfoByGid(gid)函数已经顺利执行 2. 这里再获取json，实际上检索了两次，未来可以优化
            auto tileJson = getTileJsonByGid(gid);

            // 获取碰信息：如果是SOLID类型，则添加物理组件，且图片源矩形区域就是碰撞盒大小
            if (tileInfo.type == engine::component::TileType::SOLID) {
                auto collider = std::make_unique<engine::physics::AABBCollider>(srcSize);
                gameObject->addComponent<engine::component::ColliderComponent>(std::move(collider));
                // 物理组件不受重力影响
                gameObject->addComponent<engine::component::PhysicsComponent>(&scene.getContext().getPhysicsEngine(), false);
                // 设置标签方便物理引擎检索
                gameObject->setTag("solid");
            } else if (auto rect = getColliderRect(tileJson); rect) {  
                // 如果非SOLID类型，检查自定义碰撞盒是否存在
                // 如果有，添加碰撞组件
                auto collider = std::make_unique<engine::physics::AABBCollider>(rect->size);
                auto* cc = gameObject->addComponent<engine::component::ColliderComponent>(std::move(collider));
                cc->setOffset(rect->position);  // 自定义碰撞盒的坐标是相对于图片坐标，也就是针对Transform的偏移量
                // 和物理组件（默认不受重力影响）
                gameObject->addComponent<engine::component::PhysicsComponent>(&scene.getContext().getPhysicsEngine(), false);
            }

            // 获取标签信息并设置
            auto tag = getTileProperty<std::string>(tileJson, "tag");
            if (tag) {
                gameObject->setTag(tag.value());
            } else if (tileInfo.type == engine::component::TileType::HAZARD) {
                // 如果是危险瓦片，且没有手动设置标签，则自动设置标签为 "hazard"
                gameObject->setTag("hazard");
            }

            // 获取重力信息并设置
            auto gravity = getTileProperty<bool>(tileJson, "gravity");
            if (gravity) {
                auto pc = gameObject->getComponent<engine::component::PhysicsComponent>();
                if (pc) {
                    pc->setUseGravity(gravity.value());
                } else {
                    spdlog::warn("LEVELLOADER::loadObjectLayer::WARN::对象 '{}' 在设置重力信息时没有物理组件，请检查地图设置。", objectName);
                    gameObject->addComponent<engine::component::PhysicsComponent>(&scene.getContext().getPhysicsEngine(), gravity.value());
                }
            }

            // 获取动画信息并设置
            auto animString = getTileProperty<std::string>(tileJson, "animation");
            if (animString) {
                // 解析string为JSON对象
                nlohmann::json animJson;
                try {
                    animJson = nlohmann::json::parse(animString.value());
                } catch (const nlohmann::json::parse_error& e) {
                    spdlog::error("解析动画 JSON 字符串失败: {}", e.what());
                    continue;  // 跳过此对象
                }
                // 添加AnimationComponent
                auto* ac = gameObject->addComponent<engine::component::AnimationComponent>();
                // 添加动画到 AnimationComponent
                addAnimation(animJson, ac, srcSize);
            }

            // 获取生命值信息并设置
            auto health = getTileProperty<int>(tileJson, "health");
            if (health) {
                // 添加 HealthComponent
                gameObject->addComponent<engine::component::HealthComponent>(health.value());
            }

            // 添加到场景中
            scene.addGameObject(std::move(gameObject));
            spdlog::info("LEVELLOADER::loadObjectLayer::INFO::加载对象: '{}' 完成", objectName);
        }
    }
}

void LevelLoader::addAnimation(const nlohmann::json &animJson, engine::component::AnimationComponent *ac, const glm::vec2 &spriteSize) {
    // 检查 animJson 必须是一个对象，并且 ac 不能为 nullptr
    if (!animJson.is_object() || !ac) {
        spdlog::error("无效的动画 JSON 或 AnimationComponent 指针。");
        return;
    }
    // 遍历动画 JSON 对象中的每个键值对（动画名称 : 动画信息）
    for (const auto& anim : animJson.items()) {
        std::string_view animName = anim.key();
        const auto& animInfo = anim.value();
        if (!animInfo.is_object()) {
            spdlog::warn("动画 '{}' 的信息无效或为空。", animName);
            continue;
        }
        // 获取可能存在的动画帧信息
        auto durationMS = animInfo.value("duration", 100);        // 默认持续时间为100毫秒
        auto duration = static_cast<float>(durationMS) / 1000.0f;  // 转换为秒
        auto row = animInfo.value("row", 0);                       // 默认行数为0
        // 帧信息（数组）是必须存在的
        if (!animInfo.contains("frames") || !animInfo["frames"].is_array()) {
            spdlog::warn("动画 '{}' 缺少 'frames' 数组。", animName);
            continue;
        }
        // 创建一个Animation对象 (默认为循环播放)
        auto animation = std::make_unique<engine::render::Animation>(animName);

        // 遍历数组并进行添加帧信息到animation对象
        for (const auto& frame : animInfo["frames"]) {
            if (!frame.is_number_integer()) {
                spdlog::warn("动画 {} 中 frames 数组格式错误！", animName);
                continue;;
            }
            auto column = frame.get<int>();
            // 计算源矩形
            SDL_FRect srcRect = { 
                column * spriteSize.x, 
                row * spriteSize.y, 
                spriteSize.x, 
                spriteSize.y 
            };
            // 添加动画帧到 Animation
            animation->addFrame(srcRect, duration);
        }
        // 将 Animation 对象添加到 AnimationComponent 中
        ac->addAnimation(std::move(animation));
    }
}

std::optional<utils::Rect> LevelLoader::getColliderRect(const nlohmann::json &tileJson) {
    if (!tileJson.contains("objectgroup")) return std::nullopt;
    auto& objectgroup = tileJson["objectgroup"];
    if (!objectgroup.contains("objects")) return std::nullopt;
    auto& objects = objectgroup["objects"];
    for (const auto& object : objects) {    // 一个图片只支持一个碰撞器。如果有多个，则返回第一个不为空的
        auto rect = utils::Rect(glm::vec2(object.value("x", 0.0f), object.value("y", 0.0f)), glm::vec2(object.value("width", 0.0f), object.value("height", 0.0f)));
        if (rect.size.x > 0 && rect.size.y > 0) {
            return rect;
        }
    }
    return std::nullopt;    // 如果没找到碰撞器，则返回空
}

component::TileType LevelLoader::getTileType(const nlohmann::json &tileJson) {
    if (tileJson.contains("properties")) {
        auto& properties = tileJson["properties"];
        for (auto& property : properties) {
            if (property.contains("name") && property["name"] == "solid") {
                auto isSolid = property.value("value", false);
                return isSolid ? engine::component::TileType::SOLID : engine::component::TileType::NORMAL;
            } else if (property.contains("name") && property["name"] == "slope") {
                auto slopeType = property.value("value", "");
                if (slopeType == "0_1") {
                    return engine::component::TileType::SLOPE_0_1;
                } else if (slopeType == "1_0") {
                    return engine::component::TileType::SLOPE_1_0;
                } else if (slopeType == "0_2") {
                    return engine::component::TileType::SLOPE_0_2;
                } else if (slopeType == "2_0") {
                    return engine::component::TileType::SLOPE_2_0;
                } else if (slopeType == "2_1") {
                    return engine::component::TileType::SLOPE_2_1;
                } else if (slopeType == "1_2") {
                    return engine::component::TileType::SLOPE_1_2;
                } else {
                    spdlog::error("LEVELLOADER::getTileType::ERROR::未知的斜坡类型: {}", slopeType);
                    return engine::component::TileType::NORMAL;
                }
            } else if (property.contains("name") && property["name"] == "unisolid") {
                auto isUnisolid = property.value("value", false);
                return isUnisolid ? engine::component::TileType::UNISOLID : engine::component::TileType::NORMAL;
            } else if (property.contains("name") && property["name"] == "hazard") {
                auto isHazard = property.value("value", false);
                return isHazard ? engine::component::TileType::HAZARD : engine::component::TileType::NORMAL;
            } else if (property.contains("name") && property["name"] == "ladder") {
                auto isLadder = property.value("value", false);
                return isLadder ? engine::component::TileType::LADDER : engine::component::TileType::NORMAL;
            }
            // TODO: 可以在这里添加更多的自定义属性处理逻辑
        }
    }
    return engine::component::TileType::NORMAL;
}
engine::component::TileType LevelLoader::getTileTypeByID(const nlohmann::json &tilesetJson, int localID) {
    if (tilesetJson.contains("tiles")) {
        auto& tiles = tilesetJson["tiles"];
        for (auto& tile : tiles) {
            if (tile.contains("id") && tile["id"] == localID) {
                return getTileType(tile);
            }
        }
    }
    return engine::component::TileType::NORMAL;
}
engine::component::TileInfo LevelLoader::getTileInfoByGid(int gid)
{
    if (gid == 0) {
        return engine::component::TileInfo();
    }
    // upper_bound：查找m_tilesetData中键大于 gid 的第一个元素，返回迭代器
    auto tilesetIt = m_tilesetData.upper_bound(gid);
    if (tilesetIt == m_tilesetData.begin()) {
        spdlog::error("LEVELLOADER::getTileInfoByGid::ERROR::gid为 {} 的瓦片未找到图块集。", gid);
        return engine::component::TileInfo();
    } 
    --tilesetIt;  // 前移一个位置，这样就得到不大于gid的最近一个元素（我们需要的）

    const auto& tileset = tilesetIt->second;
    auto localID = gid - tilesetIt->first;        // 计算瓦片在图块集中的局部ID
    std::string filePath = tileset.value("file_path", "");       // 获取图块集文件路径
    if (filePath.empty()) {
        spdlog::error("LEVELLOADER::getTileInfoByGid::ERROR::Tileset 文件 '{}' 缺少 'file_path' 属性。", tilesetIt->first);
        return engine::component::TileInfo();
    }
    // 图块集分为两种情况，需要分别考虑
    if (tileset.contains("image")) {    // 这是单一图片的情况
        // 获取图片路径
        auto textureID = resolvePath(tileset["image"].get<std::string>(), filePath);
        // 计算瓦片在图片网格中的坐标
        auto coordinateX = localID % tileset["columns"].get<int>();
        auto coordinateY = localID / tileset["columns"].get<int>();
        // 根据坐标确定源矩形
        SDL_FRect textureRect = {
            static_cast<float>(coordinateX * m_tileSize.x),
            static_cast<float>(coordinateY * m_tileSize.y),
            static_cast<float>(m_tileSize.x),
            static_cast<float>(m_tileSize.y)
        };
        engine::render::Sprite sprite{textureID, textureRect};
        auto tileType = getTileTypeByID(tileset, localID);   // 获取瓦片类型（只有瓦片id，还没找具体瓦片json）
        return engine::component::TileInfo(sprite, tileType);
    } else {   // 这是多图片的情况
        if (!tileset.contains("tiles")) {   // 没有tiles字段的话不符合数据格式要求，直接返回空的瓦片信息
            spdlog::error("LEVELLOADER::getTileInfoByGid::ERROR::Tileset 文件 '{}' 缺少 'tiles' 属性。", tilesetIt->first);
            return engine::component::TileInfo();
        }
        // 遍历tiles数组，根据id查找对应的瓦片
        const auto& tilesJson = tileset["tiles"];
        for (const auto& tileJson : tilesJson) {
            auto tileID = tileJson.value("id", 0);
            if (tileID == localID) {   // 找到对应的瓦片，进行后续操作
                if (!tileJson.contains("image")) {   // 没有image字段的话不符合数据格式要求，直接返回空的瓦片信息
                    spdlog::error("LEVELLOADER::getTileInfoByGid::ERROR::Tileset 文件 '{}' 中瓦片 {} 缺少 'image' 属性。", tilesetIt->first, tileID);
                    return engine::component::TileInfo();
                }
                // --- 接下来根据必要信息创建并返回 TileInfo ---
                // 获取图片路径
                auto textureID = resolvePath(tileJson["image"].get<std::string>(), filePath);
                // 先确认图片尺寸
                auto imageWidth = tileJson.value("imagewidth", 0);
                auto imageHeight = tileJson.value("imageheight", 0);
                // 从json中获取源矩形信息
                SDL_FRect textureRect = {      // tiled中源矩形信息只有设置了才会有值，没有就是默认值
                    static_cast<float>(tileJson.value("x", 0)),
                    static_cast<float>(tileJson.value("y", 0)),
                    static_cast<float>(tileJson.value("width", imageWidth)),    // 如果未设置，则使用图片尺寸
                    static_cast<float>(tileJson.value("height", imageHeight))
                };
                engine::render::Sprite sprite{textureID, textureRect};
                auto tileType = getTileType(tileJson);    // 获取瓦片类型（已经有具体瓦片json了）
                return engine::component::TileInfo(sprite, tileType);
            }
        }
    }
    // 如果能走到这里，说明查找失败，返回空的瓦片信息
    spdlog::error("LEVELLOADER::getTileInfoByGid::ERROR::图块集 '{}' 中未找到gid为 {} 的瓦片。", tilesetIt->first, gid);
    return engine::component::TileInfo();
}

std::optional<nlohmann::json> LevelLoader::getTileJsonByGid(int gid) const {
    // 1. 查找m_tilesetData中键小于等于gid的最近元素
    auto tilesetIt = m_tilesetData.upper_bound(gid);
    if (tilesetIt == m_tilesetData.begin()) {
        spdlog::error("LEVELLOADER::getTileJsonByGid::ERROR::gid为 {} 的瓦片未找到图块集。", gid);
        return std::nullopt;
    }
    --tilesetIt;
    // 2. 获取图块集json对象
    const auto& tileset = tilesetIt->second;
    auto localID = gid - tilesetIt->first;        // 计算瓦片在图块集中的局部ID
    if (!tileset.contains("tiles")) {   // 没有tiles字段的话不符合数据格式要求，直接返回空
        spdlog::error("LEVELLOADER::getTileJsonByGid::ERROR::Tileset 文件 '{}' 缺少 'tiles' 属性。", tilesetIt->first);
        return std::nullopt;
    }
    // 3. 遍历tiles数组，根据id查找对应的瓦片并返回瓦片json
    const auto& tilesJson = tileset["tiles"];
    for (const auto& tileJson : tilesJson) {
        auto tileID = tileJson.value("id", 0);
        if (tileID == localID) {   // 找到对应的瓦片，返回瓦片json
            return std::make_optional(tileJson);
        }
    }
    return std::nullopt;
}

void LevelLoader::loadTileset(std::string_view tilesetPath, int firstGid) {
    auto path = std::filesystem::path(tilesetPath);
    std::ifstream tilesetFile(path);
    if (!tilesetFile.is_open()) {
        spdlog::error("LEVELLOADER::loadTileset::ERROR::无法打开 Tileset 文件: {}", tilesetPath);
        return;
    }

    nlohmann::json tsJson;
    try {
        tilesetFile >> tsJson;
    } catch (const nlohmann::json::parse_error& e) {
        spdlog::error("LEVELLOADER::loadTileset::ERROR::解析 Tileset JSON 文件 '{}' 失败: {} (at byte {})", tilesetPath, e.what(), e.byte);
        return;
    }
    tsJson["file_path"] = tilesetPath;    // 将文件路径存储到json中，后续解析图片路径时需要
    m_tilesetData[firstGid] = std::move(tsJson);
    spdlog::info("LEVELLOADER::loadTileset::INFO::Tileset 文件 '{}' 加载完成, firstgid: {}", tilesetPath, firstGid);
}

std::string LevelLoader::resolvePath(std::string_view relativePath, std::string_view filePath) {
    try {   
        // 获取地图文件的父目录（相对于可执行文件） "assets/maps/level1.tmj" -> "assets/maps"
        auto mapDir = std::filesystem::path(filePath).parent_path();
        // 合并路径（相对于可执行文件）并返回。 
        /* std::filesystem::canonical：解析路径中的当前目录（.）和上级目录（..）导航符，得到一个干净的路径 */
        auto finalPath = std::filesystem::canonical(mapDir / relativePath);
        return finalPath.string();
    } catch (const std::exception& e) {
        spdlog::error("LEVELLOADER::resolvePath::ERROR::解析路径失败: {}", e.what());
        return std::string(relativePath);
    }
}

}