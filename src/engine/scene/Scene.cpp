#include "Scene.hpp"
#include "../object/GameObject.hpp"
#include "../core/Context.hpp"
#include "../render/Camera.hpp"
#include "../physics/PhysicsEngine.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>

namespace engine::scene {

Scene::Scene(std::string_view name, engine::core::Context &context, engine::scene::SceneManager &sceneManager)
    : m_sceneName(name), m_context(context), m_sceneManager(sceneManager), m_isInitialized(false) {
    spdlog::trace("SCENE::\"{}\"场景构造完成", m_sceneName);
}

Scene::~Scene() = default;


/// @name 生命周期管理
/// @{
void Scene::init() {
    m_isInitialized = true;
    spdlog::trace("SCENE::init::\"{}\"场景初始化完成", m_sceneName);
}
void Scene::update(float deltaTime) {
    if (!m_isInitialized) return;
    // 先更新物理引擎
    m_context.getPhysicsEngine().update(deltaTime);
    // 更新相机
    m_context.getCamera().update(deltaTime);

    for (auto it = m_gameObjects.begin(); it != m_gameObjects.end();) {
        if (*it && !(*it)->isNeedRemove()) {
            (*it)->update(deltaTime, m_context);
            ++it;
        } else {
            if (*it) (*it)->clean();
            it = m_gameObjects.erase(it);
        }
    }
    processPendingAdditions();
}
void Scene::render() {
    if (!m_isInitialized) return;
    for (auto &gameObject : m_gameObjects) {
        gameObject->render(m_context);
    }
}
void Scene::handleInput() {
    if (!m_isInitialized) return;
    for (auto it = m_gameObjects.begin(); it != m_gameObjects.end();) {
        if (*it && !(*it)->isNeedRemove()) {
            (*it)->handleInput(m_context);
            ++it;
        } else {
            if (*it) (*it)->clean();
            it = m_gameObjects.erase(it);
        }
    }
}
void Scene::clean() {
    if (!m_isInitialized) return;
    for (auto &gameObject : m_gameObjects) {
        gameObject->clean();
    }
    m_gameObjects.clear();
    m_isInitialized = false;
    spdlog::trace("SCENE::clean::\"{}\"场景清理完成", m_sceneName);
}
/// @}


/// @name 游戏对象管理
/// @{
void Scene::addGameObject(std::unique_ptr<engine::object::GameObject> &&gameObject) {
    if (gameObject) m_gameObjects.push_back(std::move(gameObject));
    else spdlog::warn("SCENE::addGameObject::WARN::\"{}\"场景添加游戏对象失败: 空游戏对象", m_sceneName);
}
void Scene::safeAddGameObject(std::unique_ptr<engine::object::GameObject> &&gameObject) {
    if (gameObject) m_pendingAdditions.push_back(std::move(gameObject));
    else spdlog::warn("SCENE::safeAddGameObject::WARN::\"{}\"场景安全添加游戏对象失败: 空游戏对象", m_sceneName);
}
void Scene::removeGameObject(engine::object::GameObject *gameObjectPtr) {
    if (!gameObjectPtr) {
        spdlog::warn("SCENE::removeGameObject::WARN::\"{}\"场景移除游戏对象失败: 空游戏对象指针", m_sceneName);
        return;
    }
    auto it = std::remove_if(m_gameObjects.begin(), m_gameObjects.end(), [gameObjectPtr](const std::unique_ptr<object::GameObject> &p) {
        return p.get() == gameObjectPtr;
    });
    if (it != m_gameObjects.end()) {
        (*it)->clean();
        m_gameObjects.erase(it, m_gameObjects.end());
        spdlog::trace("SCENE::removeGameObject::\"{}\"场景移除游戏对象成功: {}", m_sceneName, gameObjectPtr->getName());
    } else {
        spdlog::warn("SCENE::removeGameObject::WARN::\"{}\"场景移除游戏对象失败: 未找到游戏对象", m_sceneName);
    }
}
void Scene::safeRemoveGameObject(engine::object::GameObject *gameObjectPtr) {
    gameObjectPtr->setNeedRemove(true);
}
const std::vector<std::unique_ptr<engine::object::GameObject>> &Scene::getGameObjects() const {
    return m_gameObjects;
}
engine::object::GameObject *Scene::findGameObjectByName(std::string_view name) const {
    for (auto &gameObject : m_gameObjects) {
        if (gameObject && gameObject->getName() == std::string(name)) {
            return gameObject.get();
        }
    }
    return nullptr;
}
/// @}

/// @name Getters and Setters
/// @{
void Scene::setName(std::string_view name) {
    m_sceneName = name;
}
void Scene::setInitialized(bool initialized) {
    m_isInitialized = initialized;
}

bool Scene::isInitialized() const {
    return m_isInitialized;
}
std::string_view Scene::getName() const {
    return m_sceneName;
}
engine::core::Context &Scene::getContext() const {
    return m_context;
}
engine::scene::SceneManager &Scene::getSceneManager() const {
    return m_sceneManager;
}
std::vector<std::unique_ptr<engine::object::GameObject>> &Scene::getGameObjects() {
    return m_gameObjects;
}
/// @}


void Scene::processPendingAdditions() {
    for (auto &gemeObject : m_pendingAdditions) {
        m_gameObjects.push_back(std::move(gemeObject));
    }
    m_pendingAdditions.clear();
}

} // namespace engine::scene