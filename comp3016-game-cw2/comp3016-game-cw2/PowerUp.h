#pragma once
#ifndef POWER_UP_H
#define POWER_UP_H
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "GameObject.h"

const glm::vec2 SIZE(41.0f, 41.0f);
const glm::vec2 VELOCITY(0.0f, 80.0f);

class PowerUp : public GameObject
{
public:
    // powerup state
    std::string Type;
    float Duration;
    bool Activated;
    // constructor
    PowerUp(std::string type, glm::vec3 color, glm::vec2 position, Texture2D texture)
        : GameObject(position, SIZE, texture, color),
        Type(type), Activated(true), Duration(1.0f)
    { }
    PowerUp(std::string type, float duration)
        : Type(type), Duration(duration), Activated(false)
    { }
};

#endif
