#pragma once
#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "GameObject.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "Tetromino.h"

class GameLevel
{
public:
    // level state
    std::vector<GameObject> Board;
    std::vector<GameObject> nextTetro;
    // loads board into GameObjects
    void Load(int board[], unsigned int levelWidth, unsigned int levelHeight, unsigned int boardWidth, unsigned int boardHeight);
    // constructor
    GameLevel() { }
    // render level
    void Draw(SpriteRenderer& renderer);
private:
    // initialize level from tile data
    void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

#endif
