#include "HelpsScene.hpp"
#include "../../engine/core/Context.hpp"
#include "../../engine/input/InputManager.hpp"
#include "../../engine/scene/SceneManager.hpp"
#include "../../engine/ui/UIManager.hpp"
#include "../../engine/ui/UIImage.hpp"
#include <spdlog/spdlog.h>

namespace game::scene {

HelpsScene::HelpsScene(engine::core::Context& context, engine::scene::SceneManager& sceneManager)
    : engine::scene::Scene("HelpsScene", context, sceneManager) {
    spdlog::trace("HelpsScene 创建.");
}

void HelpsScene::init() {
    if (m_isInitialized) {
        return;
    }
    auto windowSize = glm::vec2(640.0f, 360.0f);

    // 创建帮助图片 UIImage （让它覆盖整个屏幕）
    auto helpImage = std::make_unique<engine::ui::UIImage>(
        "assets/textures/UI/instructions.png",
        glm::vec2(0.0f, 0.0f),
        windowSize
    );

    m_UIManager->addElement(std::move(helpImage));

    Scene::init();
    spdlog::trace("HelpsScene 初始化完成.");
}

void HelpsScene::handleInput() {
    if (!m_isInitialized) return;

    // 检测是否按下鼠标左键
    if (m_context.getInputManager().isActionPressed("MouseLeftClick")) {
        spdlog::debug("鼠标左键被按下, 退出 HelpsScene.");
        m_sceneManager.requestPopScene();
    }
}

} // namespace game::scene