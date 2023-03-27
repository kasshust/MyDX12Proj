#pragma once
#include <Component.h>
#include <TransformComponent.h>
#include <SimpleMath.h>
#include <vector>
#include <MakeRandom.h>
#include <ModelLoader.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class GameObject {
public:
    GameObject();
    void Update(float deltaTime);
    template<typename T>
    std::shared_ptr<T> GetComponent();
    template<typename T, typename... Args>
    std::shared_ptr<T> AddComponent(Args&&... args);
    TransformComponent& Transform() const;
    std::int64_t GetId() const { return m_Id; }

    // テスト用
    Model   m_Model;
private:
    
    std::vector<std::shared_ptr<Component>> m_components;
    std::unique_ptr<TransformComponent> m_transformComponent;
    std::uint64_t m_Id;


};