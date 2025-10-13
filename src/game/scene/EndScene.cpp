#include "EndScene.hpp"
#include "TitleScene.hpp"
#include "GameScene.hpp"
#include "../data/SessionData.hpp"
#include "../../engine/core/Context.hpp"
#include "../../engine/core/GameState.hpp"
#include "../../engine/input/InputManager.hpp"
#include "../../engine/scene/SceneManager.hpp"
#include "../../engine/ui/UIManager.hpp"
#include "../../engine/ui/UILabel.hpp"
#include "../../engine/ui/UIButton.hpp"
#include "../../engine/utils/Math.hpp"
#include <spdlog/spdlog.h>

namespace game::scene {

EndScene::EndScene(
    engine::core::Context& context,
    engine::scene::SceneManager& sceneManager,
    std::shared_ptr<game::data::SessionData> sessionData
) : engine::scene::Scene("EndScene", context, sceneManager), m_sessionData(std::move(sessionData)) {
    if (!m_sessionData) {
        spdlog::error("错误：结束场景收到了空的游戏数据！");
    }
    spdlog::trace("EndScene (胜利：{}) 创建.", m_sessionData->getIsWin() ? "是" : "否");
}

void EndScene::init() {
    if (m_isInitialized) {
        return;
    }
    // 设置游戏状态为 GameOver
    m_context.getGameState().setState(engine::core::State::GameOver);

    createUI();

    Scene::init();
    spdlog::info("EndScene 初始化完成。");
}

void EndScene::createUI() {
    auto windowSize = m_context.getGameState().getLogicalSize();
    if (!m_UIManager->init(windowSize)) {
         spdlog::error("错误：结束场景的 UI 管理器初始化失败！");
         return;
    }
    auto isWin = m_sessionData->getIsWin();

    // --- 主文字标签 ---
    std::string mainMessage = isWin ? "YOU WIN! CONGRATS!" : "YOU DIED! TRY AGAIN!";
    // 赢了是绿色，输了是红色
    engine::utils::FColor messageColor = isWin ? engine::utils::FColor{0.0f, 1.0f, 0.0f, 1.0f} : engine::utils::FColor{1.0f, 0.0f, 0.0f, 1.0f};

    auto mainLabel = std::make_unique<engine::ui::UILabel>(
        m_context.getTextRenderer(),
        mainMessage,
        "assets/fonts/VonwaonBitmap-16px.ttf",
        48,
        messageColor
    );
    // 标签居中
    glm::vec2 labelSize = mainLabel->getSize();
    glm::vec2 mainLabelPos = {(windowSize.x - labelSize.x) / 2.0f, windowSize.y * 0.3f};
    mainLabel->setPosition(mainLabelPos);
    m_UIManager->addElement(std::move(mainLabel));


    // --- 得分标签 ---
    int currentScore = m_sessionData->getCurrentScore();
    int highScore = m_sessionData->getHighScore();
    engine::utils::FColor scoreColor = {1.0f, 1.0f, 1.0f, 1.0f};
    int scoreFontSize = 24;

    // 当前得分
    std::string scoreText = "Score: " + std::to_string(currentScore);
    auto scoreLabel = std::make_unique<engine::ui::UILabel>(
        m_context.getTextRenderer(),
        scoreText,
        "assets/fonts/VonwaonBitmap-16px.ttf",
        scoreFontSize,
        scoreColor
    );
    glm::vec2 scoreLabelSize = scoreLabel->getSize();
    // x方向居中，y方向在主标签下方20像素
    glm::vec2 scoreLabelPos = {(windowSize.x - scoreLabelSize.x) / 2.0f, mainLabelPos.y + labelSize.y + 20.0f};
    scoreLabel->setPosition(scoreLabelPos);
    m_UIManager->addElement(std::move(scoreLabel));

    // --- 最高分 ---
    std::string highScoreText = "High Score: " + std::to_string(highScore);
    auto highScoreLabel = std::make_unique<engine::ui::UILabel>(
        m_context.getTextRenderer(),
        highScoreText,
        "assets/fonts/VonwaonBitmap-16px.ttf",
        scoreFontSize,
        scoreColor
    );
    glm::vec2 highScoreLabelSize = highScoreLabel->getSize();
    // x方向居中，y方向在当前得分下方10像素
    glm::vec2 highScoreLabelPos = {(windowSize.x - highScoreLabelSize.x) / 2.0f, scoreLabelPos.y + scoreLabelSize.y + 10.0f};
    highScoreLabel->setPosition(highScoreLabelPos);
    m_UIManager->addElement(std::move(highScoreLabel));

    // --- UI按钮 ---
    glm::vec2 buttonSize = {120.0f, 40.0f};        // 让按钮更大一点
    float buttonSpacing = 20.0f;
    float totalButtonWidth = buttonSize.x * 2 + buttonSpacing;

    // 按钮放在右下角，与边缘间隔30像素
    float buttonsX = windowSize.x - totalButtonWidth - 30.0f;
    float buttonsY = windowSize.y - buttonSize.y - 30.0f;

    // Back Button
    auto backButton = std::make_unique<engine::ui::UIButton>(
        m_context,
        "assets/textures/UI/buttons/Back1.png",
        "assets/textures/UI/buttons/Back2.png",
        "assets/textures/UI/buttons/Back3.png",
        glm::vec2{buttonsX, buttonsY},
        buttonSize,
        [this]() { this->onBackClick(); }
    );
    m_UIManager->addElement(std::move(backButton));

    // Restart Button
    buttonsX += buttonSize.x + buttonSpacing;
    auto restartButton = std::make_unique<engine::ui::UIButton>(
        m_context,
        "assets/textures/UI/buttons/Restart1.png",
        "assets/textures/UI/buttons/Restart2.png",
        "assets/textures/UI/buttons/Restart3.png",
        glm::vec2{buttonsX, buttonsY},
        buttonSize,
        [this]() { this->onRestartClick(); }
    );
    m_UIManager->addElement(std::move(restartButton));
}

void EndScene::onBackClick() {
    spdlog::info("返回按钮被点击。");
    m_sceneManager.requestReplaceScene(std::make_unique<TitleScene>(m_context, m_sceneManager, m_sessionData));
}

void EndScene::onRestartClick() {
    spdlog::info("重新开始按钮被点击。");
    // 重新开始游戏
    m_sessionData->reset();
    m_sceneManager.requestReplaceScene(std::make_unique<GameScene>(m_context, m_sceneManager, m_sessionData));
}

} // namespace game::scene