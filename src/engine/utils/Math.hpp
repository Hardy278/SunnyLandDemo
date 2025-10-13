#pragma once
#include <glm/glm.hpp>

namespace engine::utils {

struct Rect {
    glm::vec2 position;
    glm::vec2 size;
    // 添加构造函数
    Rect(const glm::vec2& pos, const glm::vec2& sz) : position(pos), size(sz) {}
};

struct FColor {
    float r;
    float g;
    float b;
    float a;
};
}