#include "Config.hpp"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>

namespace engine::core {

Config::Config(const std::string_view filePath) {
    loadFromFile(filePath);
}

bool Config::loadFromFile(const std::string_view filePath) {
    auto path = std::filesystem::path(filePath);    // 将string_view转换为文件路径 (或std::sring)
    std::ifstream file(path);
    if (!file.is_open()) {
        spdlog::warn("CONFIG::loadFromFile::配置文件 {} 未找到, 使用默认配置创建文件", filePath);
        if (!saveToFile(filePath)) {
            spdlog::error("CONFIG::loadFromFile::创建配置文件 {} 失败", filePath);
            return false;
        }
        return false;
    }
    try {
        nlohmann::json j;
        file >> j;
        fromJson(j);
        spdlog::info("CONFIG::loadFromFile::配置文件 {} 加载成功", filePath);
        return true;
    } catch (const std::exception &e) {
        spdlog::error("CONFIG::loadFromFile::配置文件 {} 解析失败: {} , 使用默认配置", filePath, e.what());
    }
    return false;
}

bool Config::saveToFile(std::string_view filepath) {
    auto path = std::filesystem::path(filepath);    // 将string_view转换为文件路径
    std::ofstream file(path);
    if (!file.is_open()) {
        spdlog::error("CONFIG::saveToFile::无法打开配置文件 '{}' 进行写入", filepath);
        return false;
    }
    try {
        nlohmann::ordered_json j = toJson();
        file << j.dump(4);
        spdlog::info("CONFIG::saveToFile::成功将配置保存到 '{}'", filepath);
        return true;
    } catch (const std::exception& e) {
        spdlog::error("CONFIG::saveToFile::写入配置文件 '{}' 时出错：{}", filepath, e.what());
    }
    return false;
}

void Config::fromJson(const nlohmann::json& j) {
    if (j.contains("window")) {
        const auto& window_config = j["window"];
        m_windowTitle = window_config.value("title", m_windowTitle);
        m_windowWidth = window_config.value("width", m_windowWidth);
        m_windowHeight = window_config.value("height", m_windowHeight);
        m_windowResizable = window_config.value("resizable", m_windowResizable);
    }
    if (j.contains("graphics")) {
        const auto& graphics_config = j["graphics"];
        m_vsyncEnabled = graphics_config.value("vsync", m_vsyncEnabled);
    }
    if (j.contains("performance")) {
        const auto& perf_config = j["performance"];
        m_targetFPS = perf_config.value("target_fps", m_targetFPS);
        if (m_targetFPS < 0) {
            spdlog::warn("CONFIG::fromJson::目标 FPS 不能为负数. 设置为 0 ( 无限制 )");
            m_targetFPS = 0;
        }
    }
    if (j.contains("audio")) {
        const auto& audio_config = j["audio"];
        m_musicVolume = audio_config.value("music_volume", m_musicVolume);
        m_soundVolume = audio_config.value("sound_volume", m_soundVolume);
    }
    // 从 JSON 加载 input_mappings
    if (j.contains("input_mappings") && j["input_mappings"].is_object()) {
        const auto& mappings_json = j["input_mappings"];
        try {
            // 直接尝试从 JSON 对象转换为 map<string, vector<string>>
            auto input_mappings = mappings_json.get<std::unordered_map<std::string, std::vector<std::string>>>();
            // 如果成功，则将 input_mappings 移动到 input_mappings_
            m_inputMappings = std::move(input_mappings);
            spdlog::trace("CONFIG::fromJson::成功从配置加载输入映射");
        } catch (const std::exception& e) {
            spdlog::warn("CONFIG::fromJson::配置加载警告: 解析 'input_mappings' 时发生异常. 使用默认映射. 错误：{}", e.what());
        }
    } else {
        spdlog::trace("CONFIG::fromJson::配置跟踪: 未找到 'input_mappings' 部分或不是对象. 使用头文件中定义的默认映射");
    }
}

nlohmann::ordered_json Config::toJson() const {
    return nlohmann::ordered_json{
        {"window", {
            {"title", m_windowTitle},
            {"width", m_windowWidth},
            {"height", m_windowHeight},
            {"resizable", m_windowResizable}
        }},
        {"graphics", {
            {"vsync", m_vsyncEnabled}
        }},
        {"performance", {
            {"target_fps", m_targetFPS}
        }},
        {"audio", {
            {"music_volume", m_musicVolume},
            {"sound_volume", m_soundVolume}
        }},
        {"input_mappings", m_inputMappings}
    };
}

} // namespace engine::core