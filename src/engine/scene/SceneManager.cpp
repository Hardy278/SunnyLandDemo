#include "SceneManager.hpp"
#include "Scene.hpp"
#include "../core/Context.hpp"

#include <spdlog/spdlog.h>

namespace engine::scene {
SceneManager::SceneManager(engine::core::Context &context) : m_context(context) {
    spdlog::trace("SCENEMANAGER::场景管理器已创建");
}

SceneManager::~SceneManager() {
    spdlog::trace("SCENEMANAGER::场景管理器已销毁");
    close(); // 关闭所有场景
}

Scene *SceneManager::getCurrentScene() const {
    if (m_sceneStack.empty()) {
        spdlog::error("SCENEMANAGER::getCurrentScene::ERROR::当前场景栈为空");
        return nullptr;
    }
    return m_sceneStack.back().get(); // 返回栈顶场景的裸指针
}

engine::core::Context &SceneManager::getContext() const {
    return m_context;
}

void SceneManager::update(float deltaTime) {
    // 只更新当前场景
    Scene *currentScene = getCurrentScene();
    if (currentScene) {
        currentScene->update(deltaTime);
    }
    processPendingActions(); // 处理挂起的操作
}

void SceneManager::render() {
    // 渲染时需要渲染所有场景，而不是只渲染当前场景
    for (auto &scene : m_sceneStack) {
        scene->render();
    }
}

void SceneManager::handleInput() {
    Scene *currentScene = getCurrentScene();
    if (currentScene) {
        currentScene->handleInput();
    }
}

void SceneManager::close() {
    spdlog::trace("SCENEMANAGER::close::TRACE::关闭场景管理器并清理所有场景");
    // 清理并移除所有场景 (从栈顶到栈底)
    while (!m_sceneStack.empty()) {
        if (m_sceneStack.back()) {
            spdlog::debug("SCENEMANAGER::close::DEBUG::清理场景: {}", m_sceneStack.back()->getName());
            m_sceneStack.back()->clean();
        }
        m_sceneStack.pop_back();
    }
}

void SceneManager::processPendingActions() {
    if (m_pendingAction == PendingAction::None) {
        return;
    }

    switch (m_pendingAction) {
        case PendingAction::Pop:
            popScene(); // 弹出场景
            break;
        case PendingAction::Replace:
            replaceScene(std::move(m_pendingScene)); // 替换场景
            break;
        case PendingAction::Push:
            pushScene(std::move(m_pendingScene)); // 压入场景
            break;
        case PendingAction::None:
            break;
    }
    m_pendingAction = PendingAction::None; // 重置挂起的操作
}

void SceneManager::pushScene(std::unique_ptr<Scene> &&scene) {
    if (!scene) {
        spdlog::error("SCENEMANAGER::pushScene::ERROR::尝试压入空场景");
        return;
    }
    spdlog::debug("SCENEMANAGER::pushScene::DEBUG::压入场景: {}", scene->getName());
    // 确保场景初始化
    if (!scene->isInitialized()) {
        scene->init();
    }
    // 压入场景至栈
    m_sceneStack.push_back(std::move(scene));
}

void SceneManager::popScene() {
    if (m_sceneStack.empty()) {
        spdlog::error("SCENEMANAGER::popScene::ERROR::尝试弹出空场景栈");
        return;
    }
    spdlog::debug("SCENEMANAGER::popScene::DEBUG::弹出场景: {}", m_sceneStack.back()->getName());
    if (m_sceneStack.back()) {
        m_sceneStack.back()->clean();
    }
    m_sceneStack.pop_back(); // 弹出场景
}

void SceneManager::replaceScene(std::unique_ptr<Scene> &&scene) {
    if (!scene) {
        spdlog::error("SCENEMANAGER::replaceScene::ERROR::尝试替换为空场景");
        return;
    }
    spdlog::debug("SCENEMANAGER::replaceScene::DEBUG::替换场景: {}", scene->getName());
    // 清理并移除所有场景
    while (!m_sceneStack.empty()) {
        if (m_sceneStack.back()) {
            m_sceneStack.back()->clean();
        }
        m_sceneStack.pop_back();
    }
    // 确保场景初始化
    if (!scene->isInitialized()) {
        scene->init();
    }
    // 压入新场景
    m_sceneStack.push_back(std::move(scene));
}

void SceneManager::requestPushScene(std::unique_ptr<Scene> &&scene) {
    m_pendingAction = PendingAction::Push;
    m_pendingScene = std::move(scene);
}

void SceneManager::requestPopScene() {
    m_pendingAction = PendingAction::Pop;
}

void SceneManager::requestReplaceScene(std::unique_ptr<Scene> &&scene) {
    m_pendingAction = PendingAction::Replace;
    m_pendingScene = std::move(scene);
}

} // namespace engine::scene