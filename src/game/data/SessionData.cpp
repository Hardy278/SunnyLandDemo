#include "SessionData.hpp"
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <glm/common.hpp>

namespace game::data {

void SessionData::setCurrentHealth(int health) {
    // 将生命值限制在 0 和 m_maxHealth 之间
    m_currentHealth = glm::clamp(health, 0, m_maxHealth);
}

void SessionData::setMaxHealth(int maxHealth) {
    if (maxHealth > 0) {
        m_maxHealth = maxHealth;
        setCurrentHealth(m_currentHealth);        // 确保当前生命值不超过新的最大生命值
    } else {
        spdlog::warn("SESSIONDATA::setMaxHealth::尝试将最大生命值设置为非正数: {}", maxHealth);
    }
}

void SessionData::addScore(int scoreToAdd) {
    m_currentScore += scoreToAdd;
    setHighScore(glm::max(m_highScore, m_currentScore)); // 如果当前分数超过最高分，则更新最高分
}

void SessionData::reset() {
    m_currentHealth = m_maxHealth;
    m_currentScore = 0;
    m_levelHealth = 3;
    m_levelScore = 0;
    m_isWin = false;
    m_mapPath = "assets/maps/level1.tmj";
    spdlog::info("SESSIONDATA::reset::重置游戏数据");
}

void SessionData::setNextLevel(std::string_view mapPath) {
    m_mapPath = mapPath;
    m_levelHealth = m_currentHealth;
    m_levelScore = m_currentScore;
}

bool SessionData::saveToFile(std::string_view filename) const {
    nlohmann::json j;
    try {
        // 将成员变量序列化到 JSON 对象中
        j["level_score"] = m_levelScore;
        j["level_health"] = m_levelHealth;
        j["max_health"] = m_maxHealth;
        j["high_score"] = m_highScore;
        j["map_path"] = m_mapPath;

        // 打开文件进行写入
        auto path = std::filesystem::path(filename);
        std::ofstream ofs(path);
        if (!ofs.is_open()) {
            spdlog::error("SESSIONDATA::saveToFile::无法打开存档文件进行写入: {}", filename);
            return false;
        }
        // 将 JSON 对象写入文件（使用4个空格进行美化输出）
        ofs << j.dump(4);
        ofs.close(); // 确保文件关闭

        spdlog::info("SESSIONDATA::saveToFile::游戏数据成功存储到: {}", filename);
        return true;
    } catch (const std::exception& e) {
        spdlog::error("SESSIONDATA::saveToFile::存档时出现错误 {}: {}", filename, e.what());
        return false;
    }
}

bool SessionData::loadFromFile(std::string_view filename) {
    try {
        // 打开文件进行读取
        auto path = std::filesystem::path(filename);
        std::ifstream ifs(path);
        if (!ifs.is_open()) {
            spdlog::warn("SESSIONDATA::loadFromFile::读档时找不到文件: {}", filename);
            // 如果存档文件不存在，这不一定是错误
            return false;
        }

        // 从文件解析 JSON 数据
        nlohmann::json j;
        ifs >> j;
        ifs.close(); // 读取完成后关闭文件

        m_currentScore = m_levelScore = j.value("level_score", 0);
        m_currentHealth = m_levelHealth = j.value("level_health", 3);
        m_maxHealth = j.value("max_health", 3); // 使用合理的默认值
        m_highScore = glm::max(j.value("high_score", 0), m_highScore);  // 文件中的最高分，与当前最高分取最大值
        m_mapPath = j.value("map_path", "assets/maps/level1.tmj"); // 默认起始地图

        spdlog::info("SESSIONDATA::loadFromFile::游戏数据成功加载: {}", filename);
        return true;
    } catch (const std::exception& e) {
        spdlog::error("SESSIONDATA::loadFromFile::读档时出现错误 {}: {}", filename, e.what());
        reset();
        return false;
    }
}

bool SessionData::syncHighScore(std::string_view filename) {
    try {
        // 打开文件进行读取
        auto path = std::filesystem::path(filename);
        std::fstream fs(path);
        if (!fs.is_open()) {
            spdlog::warn("SESSIONDATA::syncHighScore::找不到文件: {}, 无法进行同步", filename);
            return false;
        }

        // 从文件解析 JSON 数据
        nlohmann::json j;
        fs >> j;
        auto m_highScorein_file = j.value("high_score", 0);

        // 根据文件中的最高分和当前最高分来决定处理方式
        if (m_highScorein_file < m_highScore) {     // 文件中的最高分 低于 当前最高分
            j["high_score"] = m_highScore;
            fs.seekp(0);                // 文件指针回到文件开头
            fs << j.dump(4);            // 将JSON对象写入文件
            spdlog::debug("SESSIONDATA::syncHighScore::最高分高于存档文件，已将最高分保存到存档中");
        } else if (m_highScorein_file > m_highScore) {  // 文件中的最高分 高于 当前最高分
            m_highScore = m_highScorein_file;
            spdlog::debug("SESSIONDATA::syncHighScore::存档文件中的最高分高于当前最高分，已更新当前最高分");
        } else {
            spdlog::debug("SESSIONDATA::syncHighScore::存档文件中的最高分与当前最高分相同，无需更新");
        }
        fs.close();
        return true;
    } catch (const std::exception& e) {
        spdlog::error("SESSIONDATA::syncHighScore::同步最高分时出现错误 {}: {}", filename, e.what());
        return false;
    }
}

} // namespace game::state 