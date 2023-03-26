#pragma once

class Component {
public:
    virtual ~Component() {}

    // ƒƒ\ƒbƒh
    virtual void Update(float deltaTime) = 0;
};