#pragma once

class Component {
public:
    virtual ~Component() {}

    // ���\�b�h
    virtual void Update(float deltaTime) = 0;
};