#include "TilelayerComponent.hpp"
#include "../object/GameObject.hpp"
#include "../core/Context.hpp"
#include "../render/Renderer.hpp"
#include "../render/Camera.hpp"
#include "../physics/PhysicsEngine.hpp"

#include <spdlog/spdlog.h>

namespace engine::component {
TileLayerComponent::TileLayerComponent(glm::ivec2 tileSize, glm::ivec2 mapSize, std::vector<TileInfo> &&tiles)
    : m_tileSize(tileSize), m_mapSize(mapSize), m_tiles(std::move(tiles)) {
    if (m_tiles.size() != static_cast<size_t>(m_mapSize.x * m_mapSize.y)) {
        spdlog::error("TILELAYERCOMPONENT::地图尺寸与提供的瓦片向量大小不匹配。瓦片数据将被清除。");
        m_tiles.clear();
        m_mapSize = {0, 0};
    }
    spdlog::trace("TILELAYERCOMPONENT::构造完成");
}

void TileLayerComponent::init() {
    if (!m_owner) {
        spdlog::warn("TILELAYERCOMPONENT::init::TileLayerComponent 的 m_owner 未设置。");
    }
    spdlog::trace("TILELAYERCOMPONENT::init::初始化完成");
}

void TileLayerComponent::render(engine::core::Context& context) {
    if (m_tileSize.x <= 0 || m_tileSize.y <= 0) {
        return; // 防止除以零或无效尺寸
    }
    // 遍历所有瓦片
    for (int y = 0; y < m_mapSize.y; ++y) {
        for (int x = 0; x < m_mapSize.x; ++x) {
            size_t index = static_cast<size_t>(y) * m_mapSize.x + x;
            // 检查索引有效性以及瓦片是否需要渲染
            if (index < m_tiles.size() && m_tiles[index].type != TileType::EMPTY) {
                const auto& tileInfo = m_tiles[index];
                // 计算该瓦片在世界中的左上角位置 (drawSprite 预期接收左上角坐标)
                glm::vec2 tileLeftTopPos = {
                    m_offset.x + static_cast<float>(x) * m_tileSize.x,
                    m_offset.y + static_cast<float>(y) * m_tileSize.y
                };
                // 但如果图片的大小与瓦片的大小不一致，需要调整 y 坐标 (瓦片层的对齐点是左下角)
                if(static_cast<int>(tileInfo.sprite.getSourceRect()->h) != m_tileSize.y) {
                    tileLeftTopPos.y -= (tileInfo.sprite.getSourceRect()->h - static_cast<float>(m_tileSize.y));
                }
                // 执行绘制
                context.getRenderer().drawSprite(context.getCamera(), tileInfo.sprite, tileLeftTopPos);
            }
        }
    }
}

void TileLayerComponent::clean() {
    if (m_physicsEngine) {
        m_physicsEngine->unregisterCollisionLayer(this);
    }
}

const TileInfo* TileLayerComponent::getTileInfoAt(glm::ivec2 pos) const {
    if (pos.x < 0 || pos.x >= m_mapSize.x || pos.y < 0 || pos.y >= m_mapSize.y) {
        spdlog::warn("TILELAYERCOMPONENT::getTileInfoAt::瓦片坐标越界: ({}, {})", pos.x, pos.y);
        return nullptr;
    }
    size_t index = static_cast<size_t>(pos.y * m_mapSize.x + pos.x);
    // 瓦片索引不能越界
    if (index < m_tiles.size()) {
        return &m_tiles[index];
    }
    spdlog::warn("TILELAYERCOMPONENT::getTileInfoAt::瓦片索引越界: {}", index);
    return nullptr;
}

TileType TileLayerComponent::getTileTypeAt(glm::ivec2 pos) const {
    const TileInfo* info = getTileInfoAt(pos);
    return info ? info->type : TileType::EMPTY;
}

TileType TileLayerComponent::getTileTypeAtWorldPos(const glm::vec2 &worldPos) const {
    glm::vec2 relativePos = worldPos - m_offset;
    int tileX = static_cast<int>(std::floor(relativePos.x / m_tileSize.x));
    int tileY = static_cast<int>(std::floor(relativePos.y / m_tileSize.y));
    return getTileTypeAt(glm::ivec2{tileX, tileY});
}
} // namespace engine::component