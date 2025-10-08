/**
 * @file Config.hpp
 * @brief 配置管理类
 */
#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include <nlohmann/json_fwd.hpp>

namespace engine::core {

/**
 * @class Config
 * @brief 配置管理类，负责加载、保存和管理应用程序的配置。
 * 
 * 该类提供了应用程序的默认配置值，包括窗口设置、图形设置、性能设置、
 * 音频设置和键盘绑定等。可以从JSON文件加载配置，也可以将当前配置保存到JSON文件。
 * 
 * 配置类是不可拷贝和不可移动的，以确保配置对象的全局唯一性。
 */
class Config final {
public:
    /// @name --- 默认配置值 ---
    /// @{
    /**
     * @brief 窗口标题
     * 
     * 默认值为"SunnyLand"，用于设置应用程序窗口的标题栏文本。
     */
    std::string m_windowTitle = "SunnyLand";
    /**
     * @brief 窗口宽度
     * 
     * 默认值为1280像素，用于设置应用程序窗口的宽度。
     */
    int m_windowWidth = 1280;
    /**
     * @brief 窗口高度
     * 
     * 默认值为720像素，用于设置应用程序窗口的高度。
     */
    int m_windowHeight = 720;
    /**
     * @brief 窗口是否可调整大小
     * 
     * 默认值为true，允许用户调整应用程序窗口的大小。
     */
    bool m_windowResizable = true;
    /**
     * @brief 是否启用垂直同步
     * 
     * 默认值为true，启用垂直同步可以减少屏幕撕裂，但可能会限制帧率。
     */
    bool m_vsyncEnabled = true;
    /**
     * @brief 目标帧率
     * 
     * 默认值为60，设置应用程序的目标每秒帧数。
     */
    int m_targetFPS = 60;
    /**
     * @brief 背景音乐音量
     * 
     * 默认值为1.0f（最大音量），范围从0.0f（静音）到1.0f（最大音量）。
     */
    float m_musicVolume = 1.0f;
    /**
     * @brief 音效音量
     * 
     * 默认值为1.0f（最大音量），范围从0.0f（静音）到1.0f（最大音量）。
     */
    float m_soundVolume = 1.0f;
    /**
     * @brief 键盘绑定映射
     * 
     * 存储操作名称到SDL Scancode名称列表的映射，允许一个操作绑定多个按键。
     * 
     * 默认包含基本的移动、跳跃、攻击和暂停游戏的按键绑定。
     */
    std::unordered_map<std::string, std::vector<std::string>> m_inputMappings = {
        {"move_left", {"A", "Left"}},
        {"move_right", {"D", "Right"}},
        {"move_up", {"W", "Up"}},
        {"move_down", {"S", "Down"}},
        {"jump", {"J", "Space"}},
        {"attack", {"K", "MouseLeft"}},
        {"pause", {"P", "Escape"}}
    };
    /// @}

    /**
     * @brief 构造函数，从指定文件加载配置
     * 
     * @param filePath 配置文件的路径，如果文件不存在或加载失败，将使用默认配置
     */
    explicit Config(std::string_view filePath);

    /// @name 禁用移动拷贝和构造函数
    /// @{
    Config(const Config &) = delete;
    Config &operator=(const Config &) = delete;
    Config(Config &&) = delete;
    Config &operator=(Config &&) = delete;
    /// @}

    /**
     * @brief 从文件加载配置
     * 
     * @param filePath 配置文件的路径
     * @return 加载成功返回true，失败返回false
     */
    bool loadFromFile(std::string_view filePath);
    
    /**
     * @brief 将当前配置保存到文件
     * 
     * @param filePath 要保存的配置文件路径
     * @return 保存成功返回true，失败返回false
     */
    [[nodiscard]] bool saveToFile(std::string_view filePath);

private:
    /**
     * @brief 从JSON对象加载配置
     * 
     * 这是一个私有方法，由loadFromFile()调用，用于将nlohmann::json对象转换为配置类的成员变量。
     * 
     * @param j 包含配置数据的JSON对象
     */
    void fromJson(const nlohmann::json &j);
    
    /**
     * @brief 将当前配置转换为JSON对象
     * 
     * 这是一个私有方法，由saveToFile()调用，用于将配置类的成员变量转换为nlohmann::json对象。
     * 
     * @return 包含当前配置数据的有序JSON对象
     */
    nlohmann::ordered_json toJson() const;
};

} // namespace engine::core