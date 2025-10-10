#include "PhysicsEngine.hpp"
#include "Collision.hpp"
#include "../component/PhysicsComponent.hpp"
#include "../component/TransformComponent.hpp"
#include "../component/TilelayerComponent.hpp"
#include "../component/ColliderComponent.hpp"
#include "../object/GameObject.hpp"

#include <spdlog/spdlog.h>
#include <glm/common.hpp>

#include <set>

namespace engine::physics {

void PhysicsEngine::registerComponent(engine::component::PhysicsComponent* component) {
    m_components.push_back(component);
    spdlog::trace("PHYSICSENGINE::registerComponent::物理组件注册完成");
}

void PhysicsEngine::unregisterComponent(engine::component::PhysicsComponent* component) {
    // 使用 remove-erase 方法安全地移除指针
    auto it = std::remove(m_components.begin(), m_components.end(), component);
    m_components.erase(it, m_components.end());
    spdlog::trace("PHYSICSENGINE::unregisterComponent::物理组件注销完成");
}

void PhysicsEngine::registerCollisionLayer(engine::component::TileLayerComponent *layer) {
    layer->setPhysicsEngine(this); // 设置物理引擎指针
    m_collisionTileLayers.push_back(layer);
    spdlog::trace("PHYSICSENGINE::registerCollisionLayer::碰撞瓦片图层注册完成");
}

void PhysicsEngine::unregisterCollisionLayer(engine::component::TileLayerComponent* layer) {
    auto it = std::remove(m_collisionTileLayers.begin(), m_collisionTileLayers.end(), layer);
    m_collisionTileLayers.erase(it, m_collisionTileLayers.end());
    spdlog::trace("PHYSICSENGINE::unregisterCollisionLayer::碰撞瓦片图层注销完成");
}

void PhysicsEngine::update(float deltaTime) {
    // 开始前清空碰撞对
    m_collisionPairs.clear();
    m_tileTriggerEvents.clear();
    // 遍历所有注册的物理组件
    for (auto* pc : m_components) {
        if (!pc || !pc->isEnabled()) { // 检查组件是否有效和启用
            continue;
        }

        pc->resetCollisionFlags(); // 重置碰撞标志

        // 应用重力 (如果组件受重力影响)：F = g * m
        if (pc->isUseGravity()) {
            pc->addForce(m_gravity * pc->getMass());
        }
        /* 还可以添加其它力影响，比如风力、摩擦力等，目前不考虑 */
        
        // 更新速度： v += a * dt，其中 a = F / m
        pc->m_velocity += (pc->getForce() / pc->getMass()) * deltaTime;
        pc->clearForce(); // 清除当前帧的力

        // 处理瓦片层碰撞（速度和位置的更新移入此函数）
        resolveTileCollisions(pc, deltaTime);
        // 应用世界边界
        applyWorldBounds(pc);
    }
    // 处理对象间碰撞
    checkObjectCollisions();
    // 检测瓦片触发事件 (检测前已经处理完位移)
    checkTileTriggers();
}

void PhysicsEngine::checkObjectCollisions() {
    // 两层循环遍历所有包含物理组件的 GameObject
    for (size_t i = 0; i < m_components.size(); ++i) {
        auto* pcA = m_components[i];
        if (!pcA || !pcA->isEnabled()) continue;
        auto* objA = pcA->getOwner();
        if (!objA) continue;
        auto* ccA = objA->getComponent<engine::component::ColliderComponent>();
        if (!ccA || !ccA->isActive()) continue;

        for (size_t j = i + 1; j < m_components.size(); ++j) {
            auto* pcB = m_components[j];
            if (!pcB || !pcB->isEnabled()) continue;
            auto* objB = pcB->getOwner();
            if (!objB) continue;
            auto* ccB = objB->getComponent<engine::component::ColliderComponent>();
            if (!ccB || !ccB->isActive()) continue;
            /* --- 通过保护性测试后，正式执行逻辑 --- */

            if (collision::checkCollision(*ccA, *ccB)) {
                // 如果是可移动物体与SOLID物体碰撞，则直接处理位置变化，不用记录碰撞对
                if (objA->getTag() != "solid" && objB->getTag() == "solid") {
                    resolveSolidObjectCollisions(objA, objB);
                } else if (objA->getTag() == "solid" && objB->getTag() != "solid") {
                    resolveSolidObjectCollisions(objB, objA);
                } else {
                    // 记录碰撞对
                    m_collisionPairs.emplace_back(objA, objB);
                }
            }
        }
    }
}

void PhysicsEngine::resolveTileCollisions(engine::component::PhysicsComponent* pc, float deltaTime) {
    // 检查组件是否有效
    auto* obj = pc->getOwner();
    if (!obj) return;
    auto* tc = obj->getComponent<engine::component::TransformComponent>();
    auto* cc = obj->getComponent<engine::component::ColliderComponent>();
    if (!tc || !cc || cc->isTrigger()) return;
    auto worldAABB = cc->getWorldAABB();   // 使用最小包围盒进行碰撞检测（简化）
    auto objPos = worldAABB.position;
    auto objSize = worldAABB.size;
    if (worldAABB.size.x <= 0.0f || worldAABB.size.y <= 0.0f) return;
    // -- 检查结束, 正式开始处理 --
    
    constexpr float tolerance = 1.0f;       // 检查右边缘和下边缘时，需要减1像素，否则会检查到下一行/列的瓦片
    auto ds = pc->m_velocity * deltaTime;  // 计算物体在deltaTime内的位移
    auto newObjPos = objPos + ds;        // 计算物体在deltaTime后的新位置

    if (!cc->isActive()) {  // 如果碰撞器未激活，直接让物体正常移动，然后返回。
        tc->translate(ds);
        pc->m_velocity = glm::clamp(pc->m_velocity, -m_maxSpeed, m_maxSpeed);
        return;
    }

    // 遍历所有注册的碰撞瓦片层
    for (auto* layer : m_collisionTileLayers) {
        if (!layer) continue;
        auto tileSize = layer->getTileSize();
        // 轴分离碰撞检测：先检查X方向是否有碰撞 (y方向使用初始值objPos.y)
        if (ds.x > 0.0f) {
            // 检查右侧碰撞，需要分别测试右上和右下角
            auto rightTopX = newObjPos.x + objSize.x;
            auto tileX = static_cast<int>(floor(rightTopX / tileSize.x));   // 获取x方向瓦片坐标
            // y方向坐标有两个，右上和右下
            auto tileY = static_cast<int>(floor(objPos.y / tileSize.y));
            auto tileTypeTop = layer->getTileTypeAt({tileX, tileY});        // 右上角瓦片类型
            auto tileYBottom = static_cast<int>(floor((objPos.y + objSize.y - tolerance) / tileSize.y));
            auto tileTypeBottom = layer->getTileTypeAt({tileX, tileYBottom});     // 右下角瓦片类型

            if (tileTypeTop == engine::component::TileType::SOLID || tileTypeBottom == engine::component::TileType::SOLID) {
                // 撞墙了！速度归零，x方向移动到贴着墙的位置
                newObjPos.x = tileX * layer->getTileSize().x - objSize.x;
                pc->m_velocity.x = 0.0f;
                pc->setCollidedRight(true); // 设置碰撞标志
            } else {
                // 检测右下角斜坡瓦片
                auto widthRight = newObjPos.x + objSize.x - tileX * tileSize.x;
                auto heightRight = getTileHeightAtWidth(widthRight, tileTypeBottom, tileSize);
                if (heightRight > 0.0f) {
                    // 如果有碰撞（角点的世界y坐标 > 斜坡地面的世界y坐标）, 就让物体贴着斜坡表面
                    if (newObjPos.y > (tileYBottom + 1) * layer->getTileSize().y - objSize.y - heightRight) {
                        newObjPos.y = (tileYBottom + 1) * layer->getTileSize().y - objSize.y - heightRight;
                        pc->setCollidedBelow(true);    // 设置碰撞标志
                    }
                }
            }
        } else if (ds.x < 0.0f) {
            // 检查左侧碰撞，需要分别测试左上和左下角
            auto leftTopX = newObjPos.x;
            auto tileX = static_cast<int>(floor(leftTopX / tileSize.x));    // 获取x方向瓦片坐标
            // y方向坐标有两个，左上和左下
            auto tileY = static_cast<int>(floor(objPos.y / tileSize.y));
            auto tileTypeTop = layer->getTileTypeAt({tileX, tileY});        // 左上角瓦片类型
            auto tileYBottom = static_cast<int>(floor((objPos.y + objSize.y - tolerance) / tileSize.y));
            auto tileTypeBottom = layer->getTileTypeAt({tileX, tileYBottom});     // 左下角瓦片类型

            if (tileTypeTop == engine::component::TileType::SOLID || tileTypeBottom == engine::component::TileType::SOLID) {
                // 撞墙了！速度归零，x方向移动到贴着墙的位置
                newObjPos.x = (static_cast<float>(tileX) + 1.0f) * layer->getTileSize().x;
                pc->m_velocity.x = 0.0f;
                pc->setCollidedLeft(true); // 设置碰撞标志
            } else {
                // 检测左下角斜坡瓦片
                auto widthLeft = newObjPos.x - tileX * tileSize.x;
                auto heightLeft = getTileHeightAtWidth(widthLeft, tileTypeBottom, tileSize);
                if (heightLeft > 0.0f) {
                    if (newObjPos.y > (tileYBottom + 1) * layer->getTileSize().y - objSize.y - heightLeft) {
                        newObjPos.y = (tileYBottom + 1) * layer->getTileSize().y - objSize.y - heightLeft;
                        pc->setCollidedBelow(true);    // 设置碰撞标志
                    }
                }
            }
        }
        // 轴分离碰撞检测：再检查Y方向是否有碰撞 (x方向使用初始值objPos.x)
        if (ds.y > 0.0f) {
            // 检查底部碰撞，需要分别测试左下和右下角
            auto bottomLeftY = newObjPos.y + objSize.y;
            auto tileY = static_cast<int>(floor(bottomLeftY / tileSize.y));

            auto tileX = static_cast<int>(floor(objPos.x / tileSize.x));
            auto tileTypeLeft = layer->getTileTypeAt({tileX, tileY});           // 左下角瓦片类型   
            auto tileXRight = static_cast<int>(floor((objPos.x + objSize.x - tolerance) / tileSize.x));
            auto tileTypeRight = layer->getTileTypeAt({tileXRight, tileY});     // 右下角瓦片类型

            if (tileTypeLeft == engine::component::TileType::SOLID || tileTypeRight == engine::component::TileType::SOLID ||
                tileTypeLeft == engine::component::TileType::UNISOLID || tileTypeRight == engine::component::TileType::UNISOLID) {
                // 到达地面！速度归零，y方向移动到贴着地面的位置
                newObjPos.y = tileY * layer->getTileSize().y - objSize.y;
                pc->m_velocity.y = 0.0f;
                pc->setCollidedBelow(true);    // 设置碰撞标志
            // 如果两个角点都位于梯子上，则判断是不是处在梯子顶层
            } else if (tileTypeLeft == engine::component::TileType::LADDER && tileTypeRight == engine::component::TileType::LADDER) {
                auto tileTypeUpL = layer->getTileTypeAt({tileX, tileY - 1});       // 检测左角点上方瓦片类型
                auto tileTypeUpR = layer->getTileTypeAt({tileXRight, tileY - 1}); // 检测右角点上方瓦片类型
                // 如果上方不是梯子，证明处在梯子顶层
                if (tileTypeUpR != engine::component::TileType::LADDER && tileTypeUpL != engine::component::TileType::LADDER) {
                    // 通过是否使用重力来区分是否处于攀爬状态。
                    if (pc->isUseGravity()) {   // 非攀爬状态
                        pc->setOnTopLadder(true);       // 设置在梯子顶层标志
                        pc->setCollidedBelow(true);     // 设置下方碰撞标志
                        // 让物体贴着梯子顶层位置(与SOLID情况相同)
                        newObjPos.y = tileY * layer->getTileSize().y - objSize.y;
                        pc->m_velocity.y = 0.0f;
                    } else {}    // 攀爬状态，不做任何处理
                }
            } else {
                // 检测斜坡瓦片（下方两个角点都要检测）
                auto widthLeft = objPos.x - tileX * tileSize.x;
                auto widthRight = objPos.x + objSize.x - tileXRight * tileSize.x;
                auto heightLeft = getTileHeightAtWidth(widthLeft, tileTypeLeft, tileSize);
                auto heightRight = getTileHeightAtWidth(widthRight, tileTypeRight, tileSize);
                auto height = glm::max(heightLeft, heightRight);  // 找到两个角点的最高点进行检测
                if (height > 0.0f) {    // 说明至少有一个角点处于斜坡瓦片
                    if (newObjPos.y > (tileY + 1) * layer->getTileSize().y - objSize.y - height) {
                        newObjPos.y = (tileY + 1) * layer->getTileSize().y - objSize.y - height;
                        pc->m_velocity.y = 0.0f;     // 只有向下运动时才需要让 y 速度归零
                        pc->setCollidedBelow(true);    // 设置碰撞标志
                    }
                }
            }
        } else if (ds.y < 0.0f) {
            // 检查顶部碰撞，需要分别测试左上和右上角
            auto topLeftY = newObjPos.y;
            auto tileY = static_cast<int>(floor(topLeftY / tileSize.y));

            auto tileX = static_cast<int>(floor(objPos.x / tileSize.x));
            auto tileTypeLeft = layer->getTileTypeAt({tileX, tileY});        // 左上角瓦片类型
            auto tileXRight = static_cast<int>(floor((objPos.x + objSize.x - tolerance) / tileSize.x));
            auto tileTypeRight = layer->getTileTypeAt({tileXRight, tileY});     // 右上角瓦片类型

            if (tileTypeLeft == engine::component::TileType::SOLID || tileTypeRight == engine::component::TileType::SOLID) {
                // 撞到天花板！速度归零，y方向移动到贴着天花板的位置
                newObjPos.y = (static_cast<float>(tileY) + 1.0f) * layer->getTileSize().y;
                pc->m_velocity.y = 0.0f;
                pc->setCollidedAbove(true);    // 设置碰撞标志
            }
        }
    }
    // 更新物体位置，并限制最大速度
    tc->translate(newObjPos - objPos);   // 使用translate方法，避免直接设置位置，因为碰撞盒可能有偏移量
    pc->m_velocity = glm::clamp(pc->m_velocity, -m_maxSpeed, m_maxSpeed);
}

void PhysicsEngine::resolveSolidObjectCollisions(engine::object::GameObject* moveObj, engine::object::GameObject* solidObj) {
    // 进入此函数前，已经检查了各个组件的有效性，因此直接进行计算
    auto* moveTC = moveObj->getComponent<engine::component::TransformComponent>();
    auto* movePC = moveObj->getComponent<engine::component::PhysicsComponent>();
    auto* moveCC = moveObj->getComponent<engine::component::ColliderComponent>();
    auto* solidCC = solidObj->getComponent<engine::component::ColliderComponent>();

    // 这里只能获取期望位置，无法获取当前帧初始位置，因此无法进行轴分离碰撞检测
    /* 未来可以进行重构，让这里可以获取初始位置。但是我们展示另外一种处理方法 */
    auto moveAABB = moveCC->getWorldAABB();
    auto solidAABB = solidCC->getWorldAABB();

    // --- 使用最小平移向量解决碰撞问题 ---
    auto moveCenter = moveAABB.position + moveAABB.size / 2.0f;
    auto solidCenter = solidAABB.position + solidAABB.size / 2.0f;
    // 计算两个包围盒的重叠部分
    auto overlap = glm::vec2(moveAABB.size / 2.0f + solidAABB.size / 2.0f) - glm::abs(moveCenter - solidCenter);
    if (overlap.x < 0.1f && overlap.y < 0.1f) return;  // 如果重叠部分太小，则认为没有碰撞
    
    if (overlap.x < overlap.y) {    // 如果重叠部分在x方向上更小，则认为碰撞发生在x方向上（推出x方向平移向量最小）
        if (moveCenter.x < solidCenter.x) {
            // 移动物体在左边，让它贴着右边SOLID物体（相当于向左移出重叠部分），y方向正常移动
            moveTC->translate(glm::vec2(-overlap.x, 0.0f));
            // 如果速度为正(向右移动)，则归零 （if判断不可少，否则可能出现错误吸附）
            if (movePC->m_velocity.x > 0.0f) {
                movePC->m_velocity.x = 0.0f;
                movePC->setCollidedRight(true);
            }
        } else {
            // 移动物体在右边，让它贴着左边SOLID物体（相当于向右移出重叠部分），y方向正常移动
            moveTC->translate(glm::vec2(overlap.x, 0.0f));
            if (movePC->m_velocity.x < 0.0f) {
                movePC->m_velocity.x = 0.0f;
                movePC->setCollidedLeft(true);
            }
        }
    } else {                        // 重叠部分在y方向上更小，则认为碰撞发生在y方向上（推出y方向平移向量最小）
        if (moveCenter.y < solidCenter.y) {
            // 移动物体在上面，让它贴着下面SOLID物体（相当于向上移出重叠部分），x方向正常移动
            moveTC->translate(glm::vec2(0.0f, -overlap.y));
            if (movePC->m_velocity.y > 0.0f) {
                movePC->m_velocity.y = 0.0f;
                movePC->setCollidedBelow(true);
            }
        } else {
            // 移动物体在下面，让它贴着上面SOLID物体（相当于向下移出重叠部分），x方向正常移动
            moveTC->translate(glm::vec2(0.0f, overlap.y));
            if (movePC->m_velocity.y < 0.0f) {
                movePC->m_velocity.y = 0.0f;
                movePC->setCollidedAbove(true);
            }
        }
    }
}

void PhysicsEngine::applyWorldBounds(engine::component::PhysicsComponent *pc) {
    if (!pc || !m_worldBounds) return;

    // 只限定左、上、右边界，不限定下边界，以碰撞盒作为判断依据
    auto* obj = pc->getOwner();
    auto* cc = obj->getComponent<engine::component::ColliderComponent>();
    auto* tc = obj->getComponent<engine::component::TransformComponent>();
    auto worldAABB = cc->getWorldAABB();
    auto objPos = worldAABB.position;
    auto objSize = worldAABB.size;

    // 检查左边界
    if (objPos.x < m_worldBounds->position.x) {
        pc->m_velocity.x = 0.0f;
        objPos.x = m_worldBounds->position.x;
        pc->setCollidedLeft(true);
    }
    // 检查上边界
    if (objPos.y < m_worldBounds->position.y) {
        pc->m_velocity.y = 0.0f;
        objPos.y = m_worldBounds->position.y;
        pc->setCollidedAbove(true);
    }
    // 检查右边界
    if (objPos.x + objSize.x > m_worldBounds->position.x + m_worldBounds->size.x) {
        pc->m_velocity.x = 0.0f;
        objPos.x = m_worldBounds->position.x + m_worldBounds->size.x - objSize.x;
        pc->setCollidedRight(true);
    }
    // 更新物体位置(使用translate方法，新位置 - 旧位置)
    tc->translate(objPos - worldAABB.position);
}

float PhysicsEngine::getTileHeightAtWidth(float width, engine::component::TileType type, glm::vec2 tileSize) {
    auto relX = glm::clamp(width / tileSize.x, 0.0f, 1.0f);
    switch (type) {
        case engine::component::TileType::SLOPE_0_1:        // 左0  右1
            return relX * tileSize.y;
        case engine::component::TileType::SLOPE_0_2:        // 左0  右1/2
            return relX * tileSize.y * 0.5f;
        case engine::component::TileType::SLOPE_2_1:        // 左1/2右1
            return relX * tileSize.y * 0.5f + tileSize.y * 0.5f;
        case engine::component::TileType::SLOPE_1_0:        // 左1  右0
            return (1.0f - relX) * tileSize.y;
        case engine::component::TileType::SLOPE_2_0:        // 左1/2右0
            return (1.0f - relX) * tileSize.y * 0.5f;
        case engine::component::TileType::SLOPE_1_2:        // 左1  右1/2
            return (1.0f - relX) * tileSize.y * 0.5f + tileSize.y * 0.5f;
        default:
            return 0.0f;   // 默认返回0，表示没有斜坡
    }
}
void PhysicsEngine::checkTileTriggers() {
    for (auto* pc : m_components) {
        if (!pc || !pc->isEnabled()) continue;  // 检查组件是否有效和启用
        auto* obj = pc->getOwner();
        if (!obj) continue;
        auto* cc = obj->getComponent<engine::component::ColliderComponent>();
        if (!cc || !cc->isActive() || cc->isTrigger()) continue;    // 如果游戏对象本就是触发器，则不需要检查瓦片触发事件

        // 获取物体的世界AABB
        auto worldAABB = cc->getWorldAABB();

        // 使用 set 来跟踪循环遍历中已经触发过的瓦片类型，防止重复添加（例如，玩家同时踩到两个尖刺，只需要受到一次伤害）
        std::set<engine::component::TileType> triggersSet;

        // 遍历所有注册的碰撞瓦片层分别进行检测
        for (auto* layer : m_collisionTileLayers) {
            if (!layer) continue;
            auto tileSize = layer->getTileSize();
            constexpr float tolerance = 1.0f;   // 检查右边缘和下边缘时，需要减1像素，否则会检查到下一行/列的瓦片
            // 获取瓦片坐标范围
            auto startX = static_cast<int>(floor(worldAABB.position.x / tileSize.x));
            auto endX = static_cast<int>(ceil((worldAABB.position.x + worldAABB.size.x - tolerance) / tileSize.x));
            auto startY = static_cast<int>(floor(worldAABB.position.y / tileSize.y));
            auto endY = static_cast<int>(ceil((worldAABB.position.y + worldAABB.size.y - tolerance) / tileSize.y));

            // 遍历瓦片坐标范围进行检测
            for (int x = startX; x < endX; ++x) {
                for (int y = startY; y < endY; ++y) {
                    auto tileType = layer->getTileTypeAt({x, y});
                    // 未来可以添加更多触发器类型的瓦片，目前只有 HAZARD 类型
                    if (tileType == engine::component::TileType::HAZARD) {
                        triggersSet.insert(tileType);     // 记录触发事件，set 保证每个瓦片类型只记录一次
                    }
                    // 梯子类型不必记录到事件容器，物理引擎自己处理
                    else if (tileType == engine::component::TileType::LADDER) { 
                        pc->setCollidedLadder(true);
                    }
                }
            }
            // 遍历触发事件集合，添加到 m_tileTriggerEvents 中
            for (const auto& type : triggersSet) {
                m_tileTriggerEvents.emplace_back(obj, type);
            }
        }
    }
}
} // namespace engine::physics