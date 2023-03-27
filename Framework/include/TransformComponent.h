#pragma once
#include "Component.h"
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class TransformComponent : public Component {
public:
    TransformComponent();
    TransformComponent(const Matrix& transform);

    Matrix GetTransform() const { return m_transform; }
    void SetTransform(const Matrix& transform) { m_transform = transform; }

    Vector3 GetPosition() const { return m_transform.Translation(); }
    void SetPosition(const Vector3& position) {
        m_transform.Translation(position);
    }

    Quaternion GetRotation() const { 
        return Quaternion::CreateFromRotationMatrix(m_transform); 
    }

    Vector3 GetYawPitchRoll() const {
        return m_transform.ToEuler();
    }

    void SetRotation(const Vector3 YawPitchRoll) {

        Matrix mat = Matrix::Identity ;
        mat = Matrix::CreateFromYawPitchRoll(YawPitchRoll.x, YawPitchRoll.y, YawPitchRoll.z) * mat;
        mat = Matrix::CreateScale(GetScale()) * mat;
        mat.Translation(GetPosition());
        m_transform = mat;
    }

    void SetRotation(const Quaternion& rotation) {
       m_transform *= Matrix::CreateFromQuaternion(rotation);
       
    }

    Vector3 GetScale() const { return Vector3(m_transform.Right().Length(), m_transform.Up().Length(), m_transform.Forward().Length()); }

    void SetScale(const Vector3& scale) {

        if (scale.x != 0.0f)   m_transform.Right(m_transform.Right() / m_transform.Right().Length() * scale.x);
        else                m_transform.Right(m_transform.Right() / m_transform.Right().Length() * 0.001f);
        
        if (scale.y != 0)   m_transform.Up(m_transform.Up() / m_transform.Up().Length() * scale.y);
        else                m_transform.Up(m_transform.Up() / m_transform.Up().Length() * 0.001f);

        if (scale.z != 0)   m_transform.Forward(m_transform.Forward() / m_transform.Forward().Length() * scale.z);
        else                m_transform.Forward(m_transform.Forward() / m_transform.Forward().Length() * 0.001f);
    }

    // メソッド
    void Move(const Vector3& offset) {
        m_transform.Translation(m_transform.Translation() + offset);
    }

    void Rotate(const Quaternion& rotation) {
        m_transform = Matrix::CreateFromQuaternion(rotation) * m_transform;
    }

    void Update(float deltaTime) override {}

private:
    Matrix m_transform; // Transform行列
};