#pragma once
#include "TransformComponent.h"

TransformComponent::TransformComponent() {
    m_transform = Matrix::Identity;
}

TransformComponent::TransformComponent(const Matrix& transform)
    : m_transform(transform) {}
