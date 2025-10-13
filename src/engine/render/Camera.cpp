#include "Camera.hpp"
#include "../component/TransformComponent.hpp"
#include <spdlog/spdlog.h>

namespace engine::render {

Camera::Camera(const glm::vec2 &viewportSize, const glm::vec2 &position, const std::optional<engine::utils::Rect> &limitBounds)
    : m_viewportSize(viewportSize), m_position(position), m_limitBounds(limitBounds) {
    spdlog::trace("CAMERA::Camera初始化成功, 位置: {}, {}", m_position.x, m_position.y);
}

void Camera::update(float deltaTime) {
    if (m_target == nullptr)  return;
    glm::vec2 targetPos = m_target->getPosition();
    glm::vec2 desired_position = targetPos - m_viewportSize / 2.0f;      // 计算目标位置 (让目标位于视口中心)

    // 计算当前位置与目标位置的距离
    auto distance_ = glm::distance(m_position, desired_position);
    constexpr float SNAP_THRESHOLD = 1.0f; // 设置一个距离阈值  (constexpr: 编译时常量，避免每次调用都计算)

    if (distance_ < SNAP_THRESHOLD) {
        // 如果距离小于阈值，直接吸附到目标位置
        m_position = desired_position;
    } else {
        // 否则，使用线性插值平滑移动   glm::mix(a,b,t): 在向量 a 和 b 之间进行插值，t 是插值因子，范围在0到1之间。
        // 公式: (b-a)*t + a;   t = 0 时结果为 a，t = 1 时结果为 b
        m_position = glm::mix(m_position, desired_position, m_smoothSpeed * deltaTime);
        m_position = glm::vec2(glm::round(m_position.x), glm::round(m_position.y));    // 四舍五入到整数,省略的话偶尔会出现画面割裂
    }

    clampPosition();
}

void Camera::move(const glm::vec2 &offset) {
    m_position += offset;
    clampPosition();
}


/// @name 转换方法
/// @{
glm::vec2 Camera::worldToScreen(const glm::vec2 &worldPos) const {
    return worldPos - m_position;
}

glm::vec2 Camera::worldToScreenWithParallax(const glm::vec2 &worldPos, const glm::vec2 &scrollFactor) const {
    return worldToScreen(worldPos) * scrollFactor;
}

glm::vec2 Camera::screenToWorld(const glm::vec2 &screenPos) const {
    return screenPos + m_position;
}
/// @}


/// @name getter / setter
/// @{
void Camera::setPosition(const glm::vec2 &position) {
    m_position = position;
    clampPosition();
}

void Camera::setLimitBounds(std::optional<engine::utils::Rect> limitBounds) {
    m_limitBounds = std::move(limitBounds);
    clampPosition(); // 设置边界后，立即应用限制
}

void Camera::setTarget(engine::component::TransformComponent *target) {
    m_target = target;
}

const glm::vec2 &Camera::getPosition() const {
    return m_position;
}

const std::optional<engine::utils::Rect> Camera::getLimitBounds() const {
    return m_limitBounds;
}

const glm::vec2 Camera::getViewportSize() const {
    return m_viewportSize;
}
engine::component::TransformComponent *Camera::getTarget() const{
    return m_target;
}
/// @}

void Camera::clampPosition() {
    if (m_limitBounds.has_value() && m_limitBounds->size.x > 0 && m_limitBounds->size.y > 0) {
        glm::vec2 minCamPos = m_limitBounds->position;
        glm::vec2 maxCamPos = m_limitBounds->position + m_limitBounds->size - m_viewportSize; // 还要减去视口大小
        
        // 防止视口比窗口大
        maxCamPos.x = std::max(maxCamPos.x, minCamPos.x);
        maxCamPos.y = std::max(maxCamPos.y, minCamPos.y);

        m_position = glm::clamp(m_position, minCamPos, maxCamPos);
    }
}

} // namespace engine::render