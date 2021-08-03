#pragma once
#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <array>
#include "GameLevel.h"
#include "Tetromino.h"
#include "PowerUp.h"

#define BOARD_WIDTH 12
#define BOARD_HEIGHT 21
#define GAME_WIDTH 480
#define GAME_HEIGHT 840

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_OVER
};

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // game state
    GameState State;
    bool Keys[1024];
    bool KeysProcessed[1024];
    unsigned int Width, Height;
    unsigned int Score, Difficulty;
    int Board[BOARD_HEIGHT * BOARD_WIDTH];
    GameLevel Level;
    Tetromino Tetro;
    Tetromino NextTetro;
    std::vector<PowerUp> PowerUps;

    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput();
    void Update(float dt, unsigned int difficulty);
    void Render();   
    void ForcePieceDown();
    // append block to board
    void BlockToBoard();
    // check board for lines
    bool CheckForLines();
    void GameOver();
    // move lines down based on amount of lines removed (naive gravity implementation)
    void MoveLinesDown(int lineRemoved);
    // functions for power ups
    void SpawnPowerUps();
    void UpdatePowerUps(float dt);
    void ExplodeBomb(int xLoc, int yLoc);
    int GetSpeed();
};

#endif