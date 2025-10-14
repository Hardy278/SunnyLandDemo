#include "Game.hpp"
#include "Time.hpp"
#include "Config.hpp"
#include "GameState.hpp"
#include "../resource/ResourceManager.hpp"
#include "../render/Renderer.hpp"
#include "../render/Camera.hpp"
#include "../render/TextRenderer.hpp"
#include "../input/InputManager.hpp"
#include "../object/GameObject.hpp"
#include "../component/SpriteComponent.hpp"
#include "../component/TransformComponent.hpp"
#include "../physics/PhysicsEngine.hpp"
#include "../scene/SceneManager.hpp"

#include "../../game/scene/TitleScene.hpp"

#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

namespace engine::core {

Game::Game() = default;

Game::~Game() {
    if (m_isRunning) {
        spdlog::error("GAME::游戏未正常关闭，请检查代码");
        close();
    }
}

/// @name 游戏主要函数
/// @{

void Game::run() {
    if (!init()) {
        spdlog::error("GAME::游戏初始化失败");
        return;
    }
    while (m_isRunning) {
        m_time->update();
        float deltaTime = static_cast<float>(m_time->getDeltaTime());
        m_inputManager->update();

        handleEvents();
        update(deltaTime);
        render();
        // spdlog::info("FPS: {}", 1.0f / deltaTime);
    }
    close();
}

void Game::registerSceneSetup(std::function<void(engine::scene::SceneManager &)> func) {
    m_sceneSetupFunc = std::move(func);
    spdlog::trace("GAME::registerSceneSetup::已注册场景设置函数。");
}

bool Game::init() {
    spdlog::trace("GAME::init::初始化游戏...");
    if (!m_sceneSetupFunc) {
        spdlog::error("GAME::init::未注册场景设置函数，无法初始化 GameApp。");
        return false;
    }

    if (!initConfig()) return false;
    if (!initWindow()) return false;
    if (!initTime()) return false;
    if (!initResourceManager()) return false;
    if (!initRenderer()) return false;
    if (!initCamera()) return false;
    if (!initTextRenderer()) return false;
    if (!initInputManager()) return false;
    if (!initPhysicsEngine()) return false;
    if (!initGameState()) return false;
    
    if (!initContext()) return false;
    if (!initSceneManager()) return false;

    m_sceneSetupFunc(*m_sceneManager);
    m_isRunning = true;
    spdlog::trace("GAME::初始化成功。");
    return true;
}

void Game::handleEvents() {
    if (m_inputManager->shouldQuit()) {
        spdlog::info("GAME::收到来自输入管理器的退出信号");
        m_isRunning = false;
        return;
    }
    m_sceneManager->handleInput();
}

void Game::update(float deltaTime) {
    m_sceneManager->update(deltaTime);
}

void Game::render() {
    m_renderer->clearScreen();
    m_sceneManager->render();
    m_renderer->present();
}

void Game::close()  {
    spdlog::trace("GAME::关闭游戏...");

    m_sceneManager->close();
    m_resourceManager.reset();
    
    if (m_SDLRenderer) {
        SDL_DestroyRenderer(m_SDLRenderer);
        m_SDLRenderer = nullptr;
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    SDL_Quit();
    m_isRunning = false;
}
/// @}

/// @name 游戏组件管理
/// @{
bool Game::initConfig(){
    try {
        m_config = std::make_unique<engine::core::Config>("assets/config.json");
    } catch (const std::exception& e) {
        spdlog::error("GAME::initConfig::初始化配置失败: {}", e.what());
        return false;
    }
    spdlog::trace("GAME::initConfig::配置初始化成功。");
    return true;
}

bool Game::initWindow() {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        spdlog::error("GAME::initWindow::SDL初始化失败: {}", SDL_GetError());
        return false;
    }
    m_window = SDL_CreateWindow(m_config->m_windowTitle.c_str(), m_config->m_windowWidth, m_config->m_windowHeight, SDL_WINDOW_RESIZABLE);
    if (!m_window) {
        spdlog::error("GAME::initWindow::SDL窗口创建失败: {}", SDL_GetError());
        return false;
    }
    m_SDLRenderer = SDL_CreateRenderer(m_window, nullptr);
    if (!m_SDLRenderer) {
        spdlog::error("GAME::initWindow::SDL渲染器创建失败: {}", SDL_GetError());
        return false;
    }
    // 设置渲染器支持透明色
    SDL_SetRenderDrawBlendMode(m_SDLRenderer, SDL_BLENDMODE_BLEND);
    // 设置 VSync (注意: VSync 开启时，驱动程序会尝试将帧率限制到显示器刷新率，有可能会覆盖我们手动设置的 target_fps)
    int vsyncMode = m_config->m_vsyncEnabled ? SDL_RENDERER_VSYNC_ADAPTIVE : SDL_RENDERER_VSYNC_DISABLED;
    SDL_SetRenderVSync(m_SDLRenderer, vsyncMode);
    spdlog::trace("GAME::initWindow::vsync 设置为: {}", vsyncMode == SDL_RENDERER_VSYNC_ADAPTIVE ? "自适应" : "禁用");
    // 设置逻辑分辨率为窗口大小的一半（针对像素游戏）
    SDL_SetRenderLogicalPresentation(m_SDLRenderer, m_config->m_windowWidth / 2, m_config->m_windowHeight / 2, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    spdlog::trace("GAME::initWindow::SDL初始化成功");
    return true;
}

bool Game::initTime() {
    try {
        m_time = std::make_unique<Time>(m_config->m_targetFPS);
    } catch (const std::exception &e) {
        spdlog::error("GAME::initTime::时间管理器初始化失败: {}", e.what());
        return false;
    }
    m_time->setTargetFPS(m_config->m_targetFPS);
    spdlog::trace("GAME::initTime::时间管理器初始化成功, FPS: {}", m_config->m_targetFPS);
    return true;
}

bool Game::initResourceManager(){
    try {
        m_resourceManager = std::make_unique<resource::ResourceManager>(m_SDLRenderer);
    } catch (const std::exception &e) {
        spdlog::error("GAME::initResourceManager::资源管理器初始化失败: {}", e.what());
        return false;
    }
    return true;
}

bool Game::initRenderer() {
    try {
        m_renderer = std::make_unique<render::Renderer>(m_SDLRenderer, m_resourceManager.get());
    } catch (const std::exception &e) {
        spdlog::error("GAME::initRenderer::渲染器初始化失败: {}", e.what());
        return false;
    }
    return true;
}

bool Game::initCamera() {
    try {
        m_camera = std::make_unique<render::Camera>(glm::vec2(m_config->m_windowWidth / 2, m_config->m_windowHeight / 2));
    } catch (const std::exception &e) {
        spdlog::error("GAME::initCamera::相机初始化失败: {}", e.what());
        return false;
    }
    return true;
}

bool Game::initTextRenderer() {
    try {
        m_textRenderer = std::make_unique<render::TextRenderer>(m_SDLRenderer, m_resourceManager.get());
    } catch (const std::exception &e) {
        spdlog::error("GAME::initTextRenderer::文本渲染器初始化失败: {}", e.what());
        return false;
    }
    return true;
}

bool Game::initInputManager() {
    try {
        m_inputManager = std::make_unique<engine::input::InputManager>(m_SDLRenderer, m_config.get());
    } catch (const std::exception &e) {
        spdlog::error("GAME::initInputManager::输入管理器初始化失败: {}", e.what());
        return false;
    }
    return true;
}

bool Game::initPhysicsEngine() {
    try {
        m_physicsEngine = std::make_unique<engine::physics::PhysicsEngine>();
    } catch (const std::exception &e) {
        spdlog::error("GAME::initPhysicsEngine::物理引擎初始化失败: {}", e.what());
        return false;
    }
    return true;
}

bool Game::initGameState() {
    try {
        m_gameState = std::make_unique<engine::core::GameState>(m_window, m_SDLRenderer);
    } catch (const std::exception& e) {
        spdlog::error("GAME::initGameState::初始化游戏状态失败: {}", e.what());
        return false;
    }
    return true;
}

bool Game::initContext() {
    try {
        m_context = std::make_unique<engine::core::Context>(
            *m_inputManager,
            *m_renderer, 
            *m_camera, 
            *m_textRenderer,
            *m_resourceManager, 
            *m_physicsEngine,
            *m_gameState
        );
    } catch (const std::exception &e) {
        spdlog::error("GAME::initContext::上下文初始化失败: {}", e.what());
        return false;
    }
    return true;
}
bool Game::initSceneManager() {
    try {
        m_sceneManager = std::make_unique<engine::scene::SceneManager>(*m_context);
    } catch (const std::exception &e) {
        spdlog::error("GAME::initSceneManager::场景管理器初始化失败: {}", e.what());
        return false;
    }
    spdlog::trace("GAME::initSceneManager::场景管理器初始化成功");
    return true;
}
/// @}

} // namespace engine::core