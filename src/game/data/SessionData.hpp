#pragma once
#include <string>
#include <string_view>
#include <memory>
#include <nlohmann/json.hpp> 

namespace game::data {

/**
 * @brief 管理不同游戏场景之间的游戏状态
 *
 * 存储玩家生命值、分数、当前关卡等信息，
 * 使这些数据在场景切换时能够保持。
 */
class SessionData final {
private:
    int m_currentHealth = 3;
    int m_maxHealth = 3;
    int m_currentScore = 0;
    int m_highScore = 0;
    bool m_isWin = false;
    
    int m_levelHealth = 3;          ///< @brief 进入关卡时的生命值（读/存档用）
    int m_levelScore = 0;           ///< @brief 进入关卡时的得分（读/存档用）
    std::string m_mapPath = "assets/maps/level1.tmj";

public:
    SessionData() = default;
    ~SessionData() = default;

    // 删除复制和移动操作以防止意外复制
    SessionData(const SessionData&) = delete;
    SessionData& operator=(const SessionData&) = delete;
    SessionData(SessionData&&) = delete;
    SessionData& operator=(SessionData&&) = delete;

    // --- Getters ---
    int getCurrentHealth() const { return m_currentHealth; }
    int getMaxHealth() const { return m_maxHealth; }
    int getCurrentScore() const { return m_currentScore; }
    int getHighScore() const { return m_highScore; }
    int getLevelHealth() const { return m_levelHealth; }
    int getLevelScore() const { return m_levelScore; }
    std::string_view getMapPath() const { return m_mapPath; }
    bool getIsWin() const { return m_isWin; }

    // --- Setters ---
    void setCurrentHealth(int health);
    void setMaxHealth(int maxHealth);
    void addScore(int scoreToAdd);
    void setHighScore(int highScore) { m_highScore= highScore; }
    void setLevelHealth(int levelHealth) {m_levelHealth = levelHealth; }
    void setLevelScore(int levelScore) {m_levelScore = levelScore; }
    void setMapPath(std::string_view mapPath) { m_mapPath = mapPath; }
    void setIsWin(bool isWin) { m_isWin = isWin; }

    void reset();                                           ///< @brief 重置游戏数据以准备开始新游戏（保留最高分）
    void setNextLevel(std::string_view mapPath);         ///< @brief 设置下一个场景信息（地图、关卡开始时的得分生命）
    bool saveToFile(std::string_view filename) const;     ///< @brief 将当前游戏数据保存到JSON文件（存档）
    bool loadFromFile(std::string_view filename);         ///< @brief 从JSON文件中读取游戏数据（读档）
    bool syncHighScore(std::string_view filename);        ///< @brief 同步最高分(文件与当前分数取最大值)
};

} // namespace game::state