#include "InputManager.hpp"
#include "../core/Config.hpp"

#include <spdlog/spdlog.h>

#include <stdexcept>

namespace engine::input { 

InputManager::InputManager(SDL_Renderer *SDLRenderer, const engine::core::Config *config) {
    initializeMappings(config);  // 初始化输入映射
    // 检查SDL渲染器是否有效
    if (!SDLRenderer) {
        spdlog::error("INPUTMANAGER::SDL_Renderer 为空");  // 记录错误日志
        throw std::runtime_error("INPUTMANAGER::SDL_Renderer 为空");  // 抛出异常
    }
    // 获取并设置鼠标初始位置
    float x, y;
    SDL_GetMouseState(&x, &y);
    m_mousePosition = {x, y};
    // 记录初始化成功的日志
    spdlog::trace("INPUTMANAGER::SDL_Renderer 初始化成功, 鼠标位置: ({}, {})", x, y);
}

void InputManager::update() {
    // 1. 根据上一帧的值更新默认的动作状态
    for (auto& [actionName, state] : m_actionStates) {
        if (state == ActionState::PRESSED_THIS_FRAME) {
            state = ActionState::HELD_DOWN;                 // 当某个键按下不动时，并不会生成SDL_Event。
        } else if (state == ActionState::RELEASED_THIS_FRAME) {
            state = ActionState::INACTIVE;
        }
    }

    // 2. 处理所有待处理的 SDL 事件 (这将设定 action_states_ 的值)
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        processEvent(event);
    }
}

bool InputManager::isActionDown(std::string_view actionName) const {
    // C++17 引入的 “带有初始化语句的 if 语句”
    if (auto it = m_actionStates.find(std::string(actionName)); it != m_actionStates.end()) {
        return it->second == ActionState::PRESSED_THIS_FRAME || it->second == ActionState::HELD_DOWN;
    }
    return false;
}

bool InputManager::isActionPressed(std::string_view actionName) const {
    if (auto it = m_actionStates.find(std::string(actionName)); it != m_actionStates.end()) {
        return it->second == ActionState::PRESSED_THIS_FRAME;
    }
    return false;
}

bool InputManager::isActionReleased(std::string_view actionName) const {
    if (auto it = m_actionStates.find(std::string(actionName)); it != m_actionStates.end()) {
        return it->second == ActionState::RELEASED_THIS_FRAME;
    }
    return false;
}

bool InputManager::shouldQuit() const {
    return m_shouldQuit;
}

void InputManager::setShouldQuit(bool shouldQuit) {
    m_shouldQuit = shouldQuit;
}

glm::vec2 InputManager::getMousePosition() const {
    return m_mousePosition;
}

glm::vec2 InputManager::getLogicalMousePosition() const {
    glm::vec2 logicalPos;
    // 通过窗口坐标获取渲染坐标（逻辑坐标）
    SDL_RenderCoordinatesFromWindow(m_SDLrenderer, m_mousePosition.x, m_mousePosition.y, &logicalPos.x, &logicalPos.y);
    return logicalPos;
}

void InputManager::processEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP: {
            SDL_Scancode scancode = event.key.scancode;     // 获取按键的scancode
            bool isDown = event.key.down; 
            bool isRepeat = event.key.repeat;

            auto it = m_inputToActionsMap.find(scancode);
            if (it != m_inputToActionsMap.end()) {     // 如果按键有对应的action
                const std::vector<std::string>& associatedActions = it->second;
                for (const auto& actionName : associatedActions) {
                    updateActionState(actionName, isDown, isRepeat); // 更新action状态
                }
            }
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            Uint32 button = event.button.button;              // 获取鼠标按钮
            bool isDown = event.button.down;
            auto it = m_inputToActionsMap.find(button);
            if (it != m_inputToActionsMap.end()) {     // 如果鼠标按钮有对应的action
                const std::vector<std::string>& associatedActions = it->second;
                for (const auto& actionName : associatedActions) {
                    // 鼠标事件不考虑repeat, 所以第三个参数传false
                    updateActionState(actionName, isDown, false); // 更新action状态
                }
            }
            // 在点击时更新鼠标位置
            m_mousePosition = {event.button.x, event.button.y};
            break;
        }
        case SDL_EVENT_MOUSE_MOTION:        // 处理鼠标运动
            m_mousePosition = {event.motion.x, event.motion.y};
            break;
        case SDL_EVENT_QUIT:
            m_shouldQuit = true;
            break;
        default:
            break;
    }
}

void InputManager::initializeMappings(const engine::core::Config* config) {
    spdlog::trace("INPUTMANAGER::initializeMappings::初始化输入映射...");
    if (!config) {
        spdlog::error("INPUTMANAGER::initializeMappings::ERROR::Config 为空指针");
        throw std::runtime_error("INPUTMANAGER::initializeMappings::ERROR::Config 为空指针");
    }
    m_actionsToKeynameMap = config->m_inputMappings;      // 获取配置中的输入映射（动作 -> 按键名称）
    m_inputToActionsMap.clear();
    m_actionStates.clear();

    // 如果配置中没有定义鼠标按钮动作(通常不需要配置),则添加默认映射, 用于 UI
    if (m_actionsToKeynameMap.find("MouseLeftClick") == m_actionsToKeynameMap.end()) {
         spdlog::debug("INPUTMANAGER::initializeMappings::DEBUG::配置中没有定义 'MouseLeftClick' 动作,添加默认映射到 'MouseLeft'.");
         m_actionsToKeynameMap["MouseLeftClick"] = {"MouseLeft"};     // 如果缺失则添加默认映射
    }
     if (m_actionsToKeynameMap.find("MouseRightClick") == m_actionsToKeynameMap.end()) {
         spdlog::debug("INPUTMANAGER::initializeMappings::DEBUG::配置中没有定义 'MouseRightClick' 动作,添加默认映射到 'MouseRight'.");
         m_actionsToKeynameMap["MouseRightClick"] = {"MouseRight"};   // 如果缺失则添加默认映射
    }
    // 遍历 动作 -> 按键名称 的映射
    for (const auto& [actionName, keyNames] : m_actionsToKeynameMap) {
        // 每个动作对应一个动作状态，初始化为 INACTIVE
        m_actionStates[actionName] = ActionState::INACTIVE;
        spdlog::trace("INPUTMANAGER::initializeMappings::映射动作: {}", actionName);
        // 设置 "按键 -> 动作" 的映射
        for (const auto& keyName : keyNames) {
            SDL_Scancode scancode = scancodeFromString(keyName);       // 尝试根据按键名称获取scancode
            Uint32 mouseButton = mouseButtonFromString(keyName);       // 尝试根据按键名称获取鼠标按钮
            // 未来可添加其它输入类型 ...

            if (scancode != SDL_SCANCODE_UNKNOWN) {      // 如果scancode有效,则将action添加到scancode_to_actions_map_中
                m_inputToActionsMap[scancode].push_back(actionName);     
                spdlog::trace("INPUTMANAGER::initializeMappings::  映射按键: {} (Scancode: {}) 到动作: {}", keyName, static_cast<int>(scancode), actionName);
            } else if (mouseButton != 0) {             // 如果鼠标按钮有效,则将action添加到mouse_button_to_actions_map_中
                m_inputToActionsMap[mouseButton].push_back(actionName); 
                spdlog::trace("INPUTMANAGER::initializeMappings::  映射鼠标按钮: {} (Button ID: {}) 到动作: {}", keyName, static_cast<int>(mouseButton), actionName);
                // else if: 未来可添加其它输入类型 ...
            } else {
                spdlog::warn("INPUTMANAGER::initializeMappings::WARN::输入映射警告: 未知键或按钮名称 '{}' 用于动作 '{}'.", keyName, actionName);
            }
        }
    }
    spdlog::trace("INPUTMANAGER::initializeMappings::输入映射初始化完成.");
}

SDL_Scancode InputManager::scancodeFromString(std::string_view keyName) {
    return SDL_GetScancodeFromName(keyName.data());
}

Uint32 InputManager::mouseButtonFromString(std::string_view buttonName) {
    if (buttonName == "MouseLeft") return SDL_BUTTON_LEFT;
    if (buttonName == "MouseMiddle") return SDL_BUTTON_MIDDLE;
    if (buttonName == "MouseRight") return SDL_BUTTON_RIGHT;
    // SDL 还定义了 SDL_BUTTON_X1 和 SDL_BUTTON_X2
    if (buttonName == "MouseX1") return SDL_BUTTON_X1;
    if (buttonName == "MouseX2") return SDL_BUTTON_X2;
    return 0; // 0 不是有效的按钮值，表示无效
}

void InputManager::updateActionState(std::string_view actionName, bool isInputActive, bool isRepeatEvent) {
    auto it = m_actionStates.find(std::string(actionName));
    if (it == m_actionStates.end()) {
        spdlog::warn("INPUTMANAGER::updateActionState::尝试更新未注册的动作状态: {}", actionName);
        return;
    }

    if (isInputActive) { // 输入被激活 (按下)
        if (isRepeatEvent) {
            it->second = ActionState::HELD_DOWN;
        } else {            // 非重复的按下事件
            it->second = ActionState::PRESSED_THIS_FRAME;
        }
    } else { // 输入被释放 (松开)
        it->second = ActionState::RELEASED_THIS_FRAME;
    }
}

}
