#include "GameObject.h"

GameObject::GameObject() {
    m_transformComponent = std::make_unique<TransformComponent>();
    m_Id = GetRandomId();
}

void GameObject::Update(float deltaTime) {
    m_transformComponent->Update(deltaTime);
    for (auto& component : m_components) {
        component->Update(deltaTime);
    }
}

template<typename T>
std::shared_ptr<T> GameObject::GetComponent() {
    for (auto& component : m_components) {
        std::shared_ptr<T> castedComponent = std::dynamic_pointer_cast<T>(component);
        if (castedComponent) {
            return castedComponent;
        }
    }
    return nullptr;
}

template<typename T, typename... Args>
std::shared_ptr<T> GameObject::AddComponent(Args&&... args) {
    std::shared_ptr<T> newComponent = std::make_shared<T>(std::forward<Args>(args)...);
    m_components.push_back(newComponent);
    return newComponent;
}

TransformComponent& GameObject::Transform() const {
    return *m_transformComponent;
}