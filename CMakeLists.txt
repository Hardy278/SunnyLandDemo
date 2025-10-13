cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(SunnyLandDemo VERSION 0.1.0 LANGUAGES C CXX)
# 设置编译器警告级别
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()

# 设置编译输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(TARGET ${PROJECT_NAME}-${CMAKE_SYSTEM_NAME})

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 搜索并链接库
find_package(glm REQUIRED)           # 数学库
find_package(SDL3 REQUIRED)          # SDL库
find_package(SDL3_image REQUIRED)    # SDL图像库
find_package(SDL3_ttf REQUIRED)      # SDL字体库
find_package(nlohmann_json REQUIRED) # JSON库
find_package(spdlog REQUIRED)        # 日志库

set(SOURCES 
    src/main.cpp

    src/engine/component/Component.cpp
    src/engine/component/AnimationComponent.cpp
    src/engine/component/HealthComponent.cpp
    src/engine/component/ParallaxComponent.cpp
    src/engine/component/SpriteComponent.cpp
    src/engine/component/TransformComponent.cpp
    src/engine/component/TilelayerComponent.cpp
    src/engine/component/PhysicsComponent.cpp
    src/engine/component/ColliderComponent.cpp

    src/engine/core/Game.cpp
    src/engine/core/Time.cpp
    src/engine/core/Config.cpp
    src/engine/core/Context.cpp
    src/engine/core/GameState.cpp

    src/engine/input/InputManager.cpp

    src/engine/object/GameObject.cpp

    src/engine/physics/Collider.hpp
    src/engine/physics/Collision.cpp
    src/engine/physics/PhysicsEngine.cpp

    src/engine/render/Renderer.cpp
    src/engine/render/Camera.cpp
    src/engine/render/Sprite.cpp
    src/engine/render/Animation.cpp
    src/engine/render/TextRenderer.cpp

    src/engine/resource/ResourceManager.cpp
    src/engine/resource/TextureManager.cpp
    src/engine/resource/FontManager.cpp

    src/engine/scene/LevelLoader.cpp
    src/engine/scene/Scene.cpp
    src/engine/scene/SceneManager.cpp

    src/engine/UI/state/UIState.hpp
    src/engine/UI/state/UIHoverState.cpp
    src/engine/UI/state/UINormalState.cpp
    src/engine/UI/state/UIPressedState.cpp

    src/engine/UI/UIButton.cpp
    src/engine/UI/UIElement.cpp
    src/engine/UI/UIImage.cpp
    src/engine/UI/UIInteractive.cpp
    src/engine/UI/UILabel.cpp
    src/engine/UI/UIManager.cpp
    src/engine/UI/UIPanel.cpp

    src/engine/utils/Math.cpp
    src/engine/utils/Alignment.cpp

    src/game/component/AI/AIBehavior.hpp
    src/game/component/AI/JumpBehavior.cpp
    src/game/component/AI/PatrolBehavior.cpp
    src/game/component/AI/UpdownBehavior.cpp

    src/game/component/state/PlayerState.cpp
    src/game/component/state/ClimbState.cpp
    src/game/component/state/DeadState.cpp
    src/game/component/state/FallState.cpp
    src/game/component/state/HurtState.cpp
    src/game/component/state/IdleState.cpp
    src/game/component/state/JumpState.cpp
    src/game/component/state/WalkState.cpp

    src/game/component/AIComponent.cpp
    src/game/component/PlayerComponent.cpp

    src/game/data/SessionData.cpp

    src/game/scene/GameScene.cpp
    src/game/scene/TitleScene.cpp
    src/game/scene/HelpsScene.cpp
    src/game/scene/MenuScene.cpp
    src/game/scene/EndScene.cpp
)
add_executable(${TARGET} ${SOURCES})

target_link_libraries(${TARGET}
    PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        glm::glm
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)

# 设置资源文件
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")