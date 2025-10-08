#include "Component.hpp"

namespace engine::component {
void Component::setOwner(engine::object::GameObject *owner) {
    m_owner = owner;
}

engine::object::GameObject *Component::getOwner() const {
    return m_owner;
}

} // namespace engine::component