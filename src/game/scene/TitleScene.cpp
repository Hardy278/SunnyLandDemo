#include "TitleScene.hpp"
#include "../../engine/core/Context.hpp"
#include "../../engine/core/GameState.hpp"
#include "../../engine/resource/ResourceManager.hpp"
#include "../../engine/render/Camera.hpp"
#include "../../engine/input/InputManager.hpp"
#include "../../engine/ui/UIManager.hpp"
#include "../../engine/ui/UIPanel.hpp"
#include "../../engine/ui/UIImage.hpp"
#include "../../engine/ui/UIButton.hpp"
#include "../../engine/ui/UILabel.hpp"
#include "../../engine/scene/LevelLoader.hpp"
#include "../../engine/scene/SceneManager.hpp"
#include "../../engine/utils/Math.hpp"
#include "../data/SessionData.hpp"
#include "GameScene.hpp"
#include "HelpsScene.hpp"
#include <spdlog/spdlog.h>

namespace game::scene {

// 构造函数：初始化场景名称和上下文，创建 UI 管理器
TitleScene::TitleScene(
    engine::core::Context& context,
    engine::scene::SceneManager& sceneManager,
    std::shared_ptr<game::data::SessionData> sessionData
) : engine::scene::Scene("TitleScene", context, sceneManager), m_sessionData(std::move(sessionData)) {
    if (!m_sessionData) {
        spdlog::warn("TITLESCENE::TitleScene 接收到空的 SessionData，创建一个默认的 SessionData");
        m_sessionData = std::make_shared<game::data::SessionData>();
    }
    spdlog::trace("TITLESCENE::TitleScene 创建.");
}

// 初始化场景
void TitleScene::init() {
    if (m_isInitialized) {
        return;
    }
    // 加载背景地图
    engine::scene::LevelLoader levelLoader;
    if (!levelLoader.loadLevel("assets/maps/level0.tmj", *this)) {
         spdlog::error("TITLESCENE::init::加载背景失败");
         return;
    }
    m_sessionData->syncHighScore("assets/save.json");      // 更新最高分

    // 重置相机坐标，不限制边界
    m_context.getCamera().setPosition(glm::vec2(0.0f, 0.0f));
    m_context.getCamera().setLimitBounds(std::nullopt);  // 若无这一行，从GameScene返回到标题场景时，相机会限制在地图边界内

    // 创建 UI 元素
    createUI();

    Scene::init();
    spdlog::trace("TITLESCENE::init::TitleScene 初始化完成.");
}

// 创建 UI 界面元素
void TitleScene::createUI() {
    spdlog::trace("TITLESCENE::createUI::创建 TitleScene UI...");
    m_context.getGameState().setState(engine::core::State::Title);
    auto windowSize = m_context.getGameState().getLogicalSize();

    if (!m_UIManager->init(windowSize)) {
        spdlog::error("TITLESCENE::createUI::初始化 UIManager 失败!");
        return;
    }

    // 创建标题图片 (假设不知道大小)
    auto titleImage = std::make_unique<engine::ui::UIImage>("assets/textures/UI/title-screen.png");
    auto size = m_context.getResourceManager().getTextureSize(titleImage->getTextureID());
    titleImage->setSize(size * 2.0f);      // 放大为2倍

    // 水平居中
    auto titlePos = (windowSize - titleImage->getSize()) / 2.0f - glm::vec2(0.0f, 50.0f);
    titleImage->setPosition(titlePos);
    m_UIManager->addElement(std::move(titleImage));

    // --- 创建按钮面板并居中 --- (4个按钮，设定好大小、间距)
    float buttonWidth = 96.0f;
    float buttonHeight = 32.0f;
    float buttonSpacing = 20.0f;
    int numButtons = 4;

    // 计算面板总宽度
    float panelWidth = numButtons * buttonWidth + (numButtons - 1) * buttonSpacing;
    float panelHeight = buttonHeight;

    // 计算面板位置使其居中
    float panelX = (windowSize.x - panelWidth) / 2.0f;
    float panelY = windowSize.y * 0.65f;  // 垂直位置中间靠下

    auto buttonPanel = std::make_unique<engine::ui::UIPanel>(
        glm::vec2(panelX, panelY),
        glm::vec2(panelWidth, panelHeight)
    );

    // --- 创建按钮并添加到 UIPanel (位置是相对于 UIPanel 的 0,0) ---
    glm::vec2 currentButtonPos = glm::vec2(0.0f, 0.0f);
    glm::vec2 buttonSize = glm::vec2(buttonWidth, buttonHeight);

    // Start Button
    auto startButton = std::make_unique<engine::ui::UIButton>(
        m_context, 
        "assets/textures/UI/buttons/Start1.png",
        "assets/textures/UI/buttons/Start2.png",
        "assets/textures/UI/buttons/Start3.png",
        currentButtonPos,
        buttonSize,
        [this]() { this->onStartGameClick();}
    );
    buttonPanel->addChild(std::move(startButton));

    // Load Button
    currentButtonPos.x += buttonWidth + buttonSpacing;
    auto loadButton = std::make_unique<engine::ui::UIButton>(
        m_context, 
        "assets/textures/UI/buttons/Load1.png",
        "assets/textures/UI/buttons/Load2.png",
        "assets/textures/UI/buttons/Load3.png",
        currentButtonPos,
        buttonSize,
        [this]() { this->onLoadGameClick();}
    );
    buttonPanel->addChild(std::move(loadButton));

    // Helps Button
    currentButtonPos.x += buttonWidth + buttonSpacing;
    auto helpsButton = std::make_unique<engine::ui::UIButton>(
        m_context,
        "assets/textures/UI/buttons/Helps1.png",
        "assets/textures/UI/buttons/Helps2.png",
        "assets/textures/UI/buttons/Helps3.png",
        currentButtonPos,
        buttonSize,
        [this]() { this->onHelpsClick();}
    );
    buttonPanel->addChild(std::move(helpsButton));

    // Quit Button
    currentButtonPos.x += buttonWidth + buttonSpacing;
    auto quitButton = std::make_unique<engine::ui::UIButton>(
        m_context,
        "assets/textures/UI/buttons/Quit1.png",
        "assets/textures/UI/buttons/Quit2.png",
        "assets/textures/UI/buttons/Quit3.png",
        currentButtonPos,
        buttonSize,
        [this]() { this->onQuitClick();}
    );
    buttonPanel->addChild(std::move(quitButton));

    // 将 UIPanel 添加到UI管理器
    m_UIManager->addElement(std::move(buttonPanel));

    // 创建 Credits 标签
    auto creditsLabel = std::make_unique<engine::ui::UILabel>(
        m_context.getTextRenderer(),
        "SunnyLand Credits: XXX - 2025",
        "assets/fonts/VonwaonBitmap-16px.ttf",
        16,
        engine::utils::FColor{0.8f, 0.8f, 0.8f, 1.0f}
    );
    creditsLabel->setPosition(glm::vec2{(windowSize.x - creditsLabel->getSize().x) / 2.0f, windowSize.y - creditsLabel->getSize().y - 10.0f});
    m_UIManager->addElement(std::move(creditsLabel));

    spdlog::trace("TITLESCENE::createUI::TitleScene UI 创建完成.");
}

// 更新场景逻辑
void TitleScene::update(float deltaTime) {
    Scene::update(deltaTime);
    // 相机自动向右移动
    m_context.getCamera().move(glm::vec2(deltaTime * 100.0f, 0.0f));
}

// --- 按钮回调实现 --- //

void TitleScene::onStartGameClick() {
    spdlog::debug("TITLESCENE::onStartGameClick::开始游戏按钮被点击。");
    if (m_sessionData) {
        m_sessionData->reset();
    }
    m_sceneManager.requestReplaceScene(std::make_unique<GameScene>(m_context, m_sceneManager, m_sessionData));
}

void TitleScene::onLoadGameClick() {
    spdlog::debug("TITLESCENE::onLoadGameClick::加载游戏按钮被点击。");
    if (!m_sessionData) {
        spdlog::error("TITLESCENE::onLoadGameClick::游戏状态为空，无法加载。");
        return;
    }
    if (m_sessionData->loadFromFile("assets/save.json")) {
        spdlog::debug("TITLESCENE::onLoadGameClick::保存文件加载成功。开始游戏...");
        m_sceneManager.requestReplaceScene(std::make_unique<GameScene>(m_context, m_sceneManager, m_sessionData));
    } else {
        spdlog::warn("TITLESCENE::onLoadGameClick::加载保存文件失败。");
    }
}

void TitleScene::onHelpsClick() {
    spdlog::debug("TITLESCENE::onHelpsClick::帮助按钮被点击。");
    m_sceneManager.requestPushScene(std::make_unique<HelpsScene>(m_context, m_sceneManager));
}

void TitleScene::onQuitClick() {
    spdlog::debug("TITLESCENE::onQuitClick::退出按钮被点击。");
    m_sessionData->syncHighScore("assets/save.json");   // 退出前先同步最高分
    m_context.getInputManager().setShouldQuit(true);
}

} // namespace game::scenes 