#include <algorithm>
#include <sstream>
#include <iostream>
#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "PostProcessor.h"
#include <irrKlang.h>
#include "ParticleGenerator.h"
#include "TextRenderer.h"

using namespace irrklang;

// Game-related State data
SpriteRenderer* Renderer;
PostProcessor* Effects;
ParticleGenerator* Particles;
ISoundEngine* SoundEngine = createIrrKlangDevice();
TextRenderer* Text;


float ShakeTime = 0.0f;
bool IsMoveValid(Tetromino tetro, int board[]);

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_MENU), Keys(), Width(width), Height(height)
{
}

Game::~Game()
{
    delete Renderer;
    delete Effects;
    delete Particles;
    delete Text;
    SoundEngine->drop();
}

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("../comp3016-game-cw2/sprite.vert", "../comp3016-game-cw2/sprite.frag", nullptr, "sprite");
    ResourceManager::LoadShader("../comp3016-game-cw2/particle.vert", "../comp3016-game-cw2/particle.frag", nullptr, "particle");
    ResourceManager::LoadShader("../comp3016-game-cw2/post_processing.vert", "../comp3016-game-cw2/post_processing.frag", nullptr, "postprocessing");

    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

    // load textures
    ResourceManager::LoadTexture("../Textures/background.png", true, "background");
    ResourceManager::LoadTexture("../Textures/block.png", true, "block");
    ResourceManager::LoadTexture("../Textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("../Textures/bomb.png", true, "bomb");
    ResourceManager::LoadTexture("../Textures/side_background.png", true, "side_background");
    ResourceManager::LoadTexture("../Textures/particle.png", true, "particle");

    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);
    Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("../Fonts/Beatles.TTF", 24);

    // loop song
    SoundEngine->stopAllSounds();
    SoundEngine->play2D("../Audio/music.mp3", true);

    // clear power ups
    this->PowerUps.clear();

    // start score
    this->Score = 0;
    this->Difficulty = 1;

    // initialise empty board with borders  
    for (size_t i = 0; i < BOARD_HEIGHT*BOARD_WIDTH; i++)
    {
        Board[i] = 0;
    }
    for (size_t i = 0; i < BOARD_WIDTH; i++)
    {   // fill in bottom line
        Board[(BOARD_HEIGHT - 1) * BOARD_WIDTH + i] = 9;
    }
    for (size_t i = 0; i < BOARD_HEIGHT - 1; i++)
    {   // fill in sides
        Board[(BOARD_WIDTH * i)] = 9;
        Board[BOARD_WIDTH * i + BOARD_WIDTH - 1] = 9;
    }

    // load level
    GameLevel levelOne;
    levelOne.Load(this->Board, GAME_WIDTH, GAME_HEIGHT, BOARD_WIDTH, BOARD_HEIGHT);
    this->Level = levelOne;

    // load random start tetromino and next tetromino
    int randTetromino = rand() % 7;
    int randNextTetromino = rand() % 7;
    Tetromino tetromino(randTetromino, 0, GAME_WIDTH, GAME_HEIGHT, BOARD_WIDTH, BOARD_HEIGHT, false);
    Tetromino nextTetromino(randNextTetromino, 0, GAME_WIDTH, GAME_HEIGHT, BOARD_WIDTH, BOARD_HEIGHT, false);
    nextTetromino.CreateNextBlock();
    this->Tetro = tetromino;
    this->NextTetro = nextTetromino;
}

void Game::Update(float dt, unsigned int difficulty)
{
    // set current level difficulty
    this->Difficulty = difficulty;
    // decrease shake time each update based on time
    if (ShakeTime > 0.0f)
    {
        ShakeTime -= dt;
        if (ShakeTime <= 0.0f)
            Effects->Shake = false;
    }
    // update powerups if there are any
    if (this->PowerUps.size() != 0)
    {
        UpdatePowerUps(dt);
    }
    // add particle effects to bombs
    for (PowerUp& powerUp : this->PowerUps)
    {
        if (powerUp.Type == "bomb")
            Particles->Update(dt, powerUp, 2, glm::vec2(powerUp.Position[0] + 40.0f, powerUp.Position[1]));
    }
}

void Game::ProcessInput()
{
    if (this->State == GAME_ACTIVE)
    {
        // move piece side to side and down
        if (this->Keys[GLFW_KEY_LEFT])
        {
            SoundEngine->play2D("../Audio/move.wav");
            this->Tetro.OffsetX--;   
            if (!IsMoveValid(this->Tetro, this->Board))
            {
                SoundEngine->play2D("../Audio/error.wav", false);
                this->Tetro.OffsetX++;
            }
            this->Tetro.CreateBlocks(GAME_WIDTH, GAME_HEIGHT, BOARD_WIDTH, BOARD_HEIGHT, this->Tetro.Chaos);
        }
        if (this->Keys[GLFW_KEY_RIGHT])
        {
            SoundEngine->play2D("../Audio/move.wav");
            this->Tetro.OffsetX++;
            if (!IsMoveValid(this->Tetro, this->Board))
            {
                SoundEngine->play2D("../Audio/error.wav", false);
                this->Tetro.OffsetX--;
            }
            this->Tetro.CreateBlocks(GAME_WIDTH, GAME_HEIGHT, BOARD_WIDTH, BOARD_HEIGHT, this->Tetro.Chaos);
        }
        if (this->Keys[GLFW_KEY_DOWN])
        {
            SoundEngine->play2D("../Audio/move.wav");
            ForcePieceDown();
        }
        // rotate clockwise and anti clockwise
        if (this->Keys[GLFW_KEY_X] && !this->KeysProcessed[GLFW_KEY_X])
        {
            SoundEngine->play2D("../Audio/move.wav");
            this->Tetro.RotationIndex++;
            this->Tetro.SetRotation(this->Tetro.RotationIndex);
            if (!IsMoveValid(this->Tetro, this->Board))
            {
                SoundEngine->play2D("../Audio/error.wav", false);
                this->Tetro.RotationIndex--;
                this->Tetro.SetRotation(this->Tetro.RotationIndex);
            }
            this->Tetro.CreateBlocks(GAME_WIDTH, GAME_HEIGHT, BOARD_WIDTH, BOARD_HEIGHT, this->Tetro.Chaos);
            this->KeysProcessed[GLFW_KEY_X] = true;
        }
        if (this->Keys[GLFW_KEY_Z] && !this->KeysProcessed[GLFW_KEY_Z])
        {
            SoundEngine->play2D("../Audio/move.wav");
            this->Tetro.RotationIndex--;
            this->Tetro.SetRotation(this->Tetro.RotationIndex);
            if (!IsMoveValid(this->Tetro, this->Board))
            {
                SoundEngine->play2D("../Audio/error.wav", false);
                this->Tetro.RotationIndex++;
                this->Tetro.SetRotation(this->Tetro.RotationIndex);
            }
            this->Tetro.CreateBlocks(GAME_WIDTH, GAME_HEIGHT, BOARD_WIDTH, BOARD_HEIGHT, this->Tetro.Chaos);
            this->KeysProcessed[GLFW_KEY_Z] = true;
        }
        if (this->Keys[GLFW_KEY_C])
        {
            Init();
        }
    }
    else if (this->State == GAME_MENU)
    {
        if (this->Keys[GLFW_KEY_ENTER])
        {
            this->State = GAME_ACTIVE;
        }
    }
    else if (this->State == GAME_OVER)
    {
        if (this->Keys[GLFW_KEY_ENTER])
        {
            this->State = GAME_ACTIVE;
            Init();
        }
    }
}

void Game::Render()
{
    std::stringstream ssScore; ssScore << this->Score;
    if (this->State == GAME_ACTIVE || this->State == GAME_MENU || this->State == GAME_OVER)
    {
        Effects->BeginRender();
        // draw background
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(GAME_WIDTH, GAME_HEIGHT), 0.0f);
        Renderer->DrawSprite(ResourceManager::GetTexture("side_background"), glm::vec2(GAME_WIDTH, 0.0f), glm::vec2(Width - GAME_WIDTH, GAME_HEIGHT), 0.0f);
        // draw score
        Text->RenderText("Score:" + ssScore.str(), 495.0f, 415.0f, 1.0f, glm::vec3(0.1f, 0.1f, 0.1f));
        std::stringstream ssDiff;
        if (Difficulty == 19)
            ssDiff << "Max Difficulty!!";
        else
            ssDiff << this->Difficulty;
        Text->RenderText("Difficulty:" + ssDiff.str(), 495.0f, 545.0f, 1.0f, glm::vec3(0.1f * (this->Difficulty * 2), 0.1f, 0.1f));
        Text->RenderText("Next Tetromino", 565.0f, 140.0f, 1.0f, glm::vec3(0.1f, 0.1f, 0.1f));

        // draw level
        this->Level.Draw(*Renderer);
        //draw block + next block
        this->Tetro.Draw(*Renderer);
        this->NextTetro.Draw(*Renderer);
        // draw power ups + particles
        Particles->Draw();
        for (PowerUp& powerUp : this->PowerUps)
            powerUp.Draw(*Renderer);
        Effects->EndRender();
        Effects->Render(glfwGetTime());
    }
    if (this->State == GAME_MENU)
    {
        Text->RenderText("Press ENTER to start", 140.0f, Height / 2, 1.0f, glm::vec3(0.1f, 0.1f, 0.1f));
    }
    if (this->State == GAME_OVER)
    {
        Text->RenderText("GAME OVER :(", 100.0f, (Height / 2) - 200, 1.0f, glm::vec3(0.1f, 0.1f, 0.1f));
        Text->RenderText("Your score was: " + ssScore.str(), 100.0f, (Height / 2)-100, 1.0f, glm::vec3(0.1f, 0.1f, 0.1f));
        Text->RenderText("Press the ENTER key to restart", 100.0f, (Height / 2), 1.0f, glm::vec3(0.1f, 0.1f, 0.1f));
        Text->RenderText("or ESC to quit the game", 100.0f, (Height / 2)+30, 1.0f, glm::vec3(0.1f, 0.1f, 0.1f));
    }
}

void Game::ForcePieceDown()
{
    if (this->State == GAME_ACTIVE)
    {
        this->Tetro.OffsetY++;
        // when tetromino cant go any further down
        if (!IsMoveValid(this->Tetro, this->Board))
        {
            bool chaos = false;
            this->SpawnPowerUps();
            this->Tetro.OffsetY--;
            BlockToBoard();
            // see if full line has been made and act accordingly
            bool wasLineCleared = CheckForLines();

            //if line wasn't cleared with chaos active = set chaos effect on screen
            if (!wasLineCleared && this->Tetro.Chaos == true)
            {
                for (PowerUp& powerUp : this->PowerUps)
                {
                    if (powerUp.Type == "chaos" && this->NextTetro.Chaos == false)
                    {
                        powerUp.Activated = true;
                        SoundEngine->play2D("../Audio/chaos.mp3", false);
                    }
                }
            }
            //check if chaos needs to be applied to next block
            for (PowerUp& powerUp : this->PowerUps)
            {
                if (powerUp.Type == "chaos" && !this->NextTetro.Chaos && !powerUp.Activated)
                {
                    chaos = true;
                }
            }
            // load random start tetromino and next tetromino
            int randNextTetromino = rand() % 7;
            Tetromino nextTetromino(randNextTetromino, 0, GAME_WIDTH, GAME_HEIGHT, BOARD_WIDTH, BOARD_HEIGHT, chaos);
            nextTetromino.CreateNextBlock();
            this->Tetro = this->NextTetro;
            bool canSpawn = IsMoveValid(this->Tetro, this->Board);
            // check if spawn location is empty - if not then player has lost
            if (!canSpawn)
                GameOver();
            this->NextTetro = nextTetromino;
            // load new board
            this->Level.Load(this->Board, GAME_WIDTH, GAME_HEIGHT, BOARD_WIDTH, BOARD_HEIGHT);
        }
        this->Tetro.CreateBlocks(GAME_WIDTH, GAME_HEIGHT, BOARD_WIDTH, BOARD_HEIGHT, this->Tetro.Chaos);
    }
}

void Game::BlockToBoard()
{
    // append tetromino index to relative board position
    for (size_t y = 0; y < 4; y++)
    {
        for (size_t x = 0; x < 4; x++)
        {
            if (this->Tetro.Tetro[y * 4 + x] == 1)
            {
                if (this->Tetro.Chaos)
                    this->Board[(y + this->Tetro.OffsetY) * BOARD_WIDTH + (x + this->Tetro.OffsetX)] = 8;
                else
                    this->Board[(y + this->Tetro.OffsetY) * BOARD_WIDTH + (x + this->Tetro.OffsetX)] = this->Tetro.TetrominoIndex + 1;               
            }
        }
    }
}

bool Game::CheckForLines()
{
    bool isLine = true;
    int lineCounter = 0;

    for (size_t y = 0; y < BOARD_HEIGHT - 1; y++)
    {
        isLine = true;
        for (size_t x = 1; x < BOARD_WIDTH - 1; x++)
        {
            if (this->Board[y * BOARD_WIDTH + x] == 0)
            {
                isLine = false;          
            }
        }
        if (isLine)
        {
            lineCounter++;
            // change values of line to 0's
            for (size_t x = 1; x < BOARD_WIDTH - 1; x++)
            {
                this->Board[y * BOARD_WIDTH + x] = 0;
            }
            MoveLinesDown(y);
        }
    }  
    if (lineCounter==0)
    {
        return false;
    }
    else if (lineCounter==1)
    {
        ShakeTime = 0.05f;
        Effects->Shake = true;
        SoundEngine->play2D("../Audio/success1.wav", false);
        this->Score += 40 * (this->Difficulty * 0.5);
    }
    else if (lineCounter > 1 && lineCounter < 4)
    {
        ShakeTime = 0.1f;
        Effects->Shake = true;
        SoundEngine->play2D("../Audio/success2.wav", false);
        this->Score += 100 * (this->Difficulty * 0.5);
    }
    else if (lineCounter == 4)
    {
        ShakeTime = 0.15f;
        Effects->Shake = true;
        SoundEngine->play2D("../Audio/success3.wav", false);
        this->Score += 150 * (this->Difficulty * 0.5);
    }
    return true;
}

void Game::GameOver()
{
    this->State = GAME_OVER;
    this->PowerUps.clear();
    Particles->RemoveParticles();
    Effects->Chaos = false;
    SoundEngine->stopAllSounds();
    SoundEngine->play2D("../Audio/gameover.wav", false);
    SoundEngine->play2D("../Audio/gameovermusic.mp3", true);
}

void Game::MoveLinesDown(int lineRemoved)
{ 
    // for each column above line (starting from bottom)
    for (size_t y = lineRemoved; y > 0; y--)
    {
        // for each row in column
        for (size_t x = 0; x < BOARD_WIDTH; x++)
        {
            if (Board[y * BOARD_WIDTH + x] != 0 && Board[y * BOARD_WIDTH + x] != 9)
            {
                Board[(y + 1) * BOARD_WIDTH + x] = Board[y * BOARD_WIDTH + x];
                Board[y * BOARD_WIDTH + x] = 0;
            }
        }
    }
}

bool IsMoveValid(Tetromino tetro, int board[])
{
    size_t y = 0;
    // ensures that only pieces on the board are compared (if any pieces of tetromino are above the board - occurs when first created)
    if (tetro.OffsetY < 0)
    {
        y = 0 - tetro.OffsetY;
    }

    // check if tetromino array collides with any values in board array
    for (y; y < 4; y++)
    {
        for (size_t x = 0; x < 4; x++)
        {
            if (tetro.Tetro[y * 4 + x] == 1 && board[(y + tetro.OffsetY) * BOARD_WIDTH + (x + tetro.OffsetX)] != 0)
                return false;
        }
    }
    return true;
}

bool ShouldSpawn(unsigned int chance)
{
    unsigned int random = rand() % chance;
    return random == 0;
}

bool IsOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type)
{
    for (const PowerUp& powerUp : powerUps)
    {
        if (powerUp.Activated)
            if (powerUp.Type == type)
                return true;
    }
    return false;
}

glm::vec2 SetpowerUpPos()
{
    // return random x position on board
    return glm::vec2((((rand() % 10) + 1) * 40), 0.0f);
}

void Game::SpawnPowerUps()
{
    if (ShouldSpawn(20))    // 1 in 20 chance
    {
        glm::vec2 pos = SetpowerUpPos();
        this->PowerUps.push_back(PowerUp("bomb", glm::vec3(1.0f, 1.0f, 1.0f), pos, ResourceManager::GetTexture("bomb")));
    }
    if (ShouldSpawn(15) && !IsOtherPowerUpActive(this->PowerUps, "chaos"))    // 1 in 15 chance
    {
        this->PowerUps.push_back(
            PowerUp("chaos", 3.0f));
    }
}

void Game::ExplodeBomb(int xLoc, int yLoc)
{
    // remove surrounding blocks of board location
    //    + + +
    //    + x +
    //    + + +
    // play sound and animation
    SoundEngine->play2D("../Audio/bomb.mp3", false);
    ShakeTime = 0.1f;
    Effects->Shake = true;
    for (size_t i = 0; i < 3; i++)
    {
        int topValue = this->Board[((yLoc - 1) * BOARD_WIDTH) + xLoc + (i - 1)];
        int midValue = this->Board[(yLoc * BOARD_WIDTH) + xLoc + (i - 1)];
        int botValue = this->Board[((yLoc + 1) * BOARD_WIDTH) + xLoc + (i - 1)];

        if (topValue != 9)
            this->Board[((yLoc - 1) * BOARD_WIDTH) + xLoc + (i - 1)] = 0;
        if (midValue != 9)
            this->Board[(yLoc * BOARD_WIDTH) + xLoc + (i - 1)] = 0;
        if (botValue != 9)
            this->Board[((yLoc + 1) * BOARD_WIDTH) + xLoc + (i - 1)] = 0;
    }
    // update board state
    this->Level.Load(this->Board, GAME_WIDTH, GAME_HEIGHT, BOARD_WIDTH, BOARD_HEIGHT);
}

int Game::GetSpeed()
{
    if (this->State == GAME_ACTIVE)
        return 21 - (int)this->Difficulty;
    else if (this->State == GAME_MENU || this->State == GAME_OVER)
        return 21;
}

void Game::UpdatePowerUps(float dt)
{
    for (PowerUp& powerUp : this->PowerUps)
    {
        powerUp.Position += VELOCITY * dt;
        if (powerUp.Activated == true)
        {
            if (powerUp.Type == "bomb")
            {
                // check pos on grid
                int offsetX = (int)powerUp.Position[0] / 40;
                int offsetY = (int)powerUp.Position[1] / 40;
                if (offsetY > BOARD_HEIGHT)
                    offsetY = 19;

                // if block below isnt empty = explode
                int blockBelow = this->Board[(offsetY + 1) * BOARD_WIDTH + offsetX];
                if (blockBelow != 0)
                {
                    ExplodeBomb(offsetX, offsetY+1);
                    Particles->Update(100, powerUp, 0);
                    powerUp.Activated = false;
                    powerUp.Duration = 0.0f;
                }
            }
            if (powerUp.Type == "chaos")
            {
                powerUp.Duration -= dt;
                Effects->Chaos = true;
                if (powerUp.Duration <= 0.0f)
                {
                    Effects->Chaos = false;
                    powerUp.Activated = false;
                }
            }
        }        
    }
    this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
        [](const PowerUp& powerUp) { return !powerUp.Activated && powerUp.Duration <= 0.0f; }
    ), this->PowerUps.end());
}
