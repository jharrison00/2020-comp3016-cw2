#pragma once
#ifndef TETROMINO_H
#define TETROMINO_H
#include "Tetrominos.h"
#include "GameObject.h"
#include <vector>

// Class to hold all data and functions for current tetromino.
class Tetromino : public Tetrominos
{
public:
    std::vector<GameObject> Blocks; 
    bool Chaos;
    int TetrominoIndex;
    int RotationIndex;
    int OffsetX;
    int OffsetY;
    std::array<int, 16> Tetro;
    // constructor(s)
    Tetromino();
    Tetromino(int tetrominoIndex, int rotationIndex,
        unsigned int levelWidth, unsigned int levelHeight,
        unsigned int boardWidth, unsigned int boardHeight, bool isChaos);
    void Draw(SpriteRenderer& renderer);    // render block
    void CreateBlocks(unsigned int levelWidth, unsigned int levelHeight, 
        unsigned int boardWidth, unsigned int boardHeight, bool isChaos);    // create GameObjects for blocks of each tetromino
    void CreateNextBlock();
    void SetRotation(int RotationIndex);    // set RotationIndex and rotate tetro array
private:
    void Rotate3x3(int rotationIndex);
    void Rotate4x4(int rotationIndex);
};

#endif

