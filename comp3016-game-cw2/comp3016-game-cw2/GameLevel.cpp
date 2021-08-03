#include "GameLevel.h"
#include <fstream>
#include <sstream>

void GameLevel::Load(int board[], unsigned int levelWidth, unsigned int levelHeight, unsigned int boardWidth, unsigned int boardHeight)
{
	//clear old data
	this->Board.clear();
	//load from 1d array
	GameLevel level;
	unsigned int tileCode;
	std::vector<std::vector<unsigned int>> tileData;
	// read array and insert into tileData
	for (size_t y = 0; y < boardHeight; y++)
	{
		std::vector<unsigned int> row;
		for (size_t x = 0; x < boardWidth; x++)
		{
			tileCode = board[y * boardWidth + x];
			row.push_back(tileCode);
		}
		tileData.push_back(row);
	}
	if (tileData.size() > 0)
	{
		this->init(tileData, levelWidth, levelHeight);
	}
}

void GameLevel::Draw(SpriteRenderer& renderer)
{
    for (GameObject& tile : this->Board)
    {
        tile.Draw(renderer);
    }
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{
    // calculate dimensions
    unsigned int height = tileData.size();
    unsigned int width = tileData[0].size(); 
    float unit_width = levelWidth / static_cast<float>(width), unit_height = levelHeight / height;
    // initialize level tiles based on tileData		
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            // check block type from level data (2D level array)
            if (tileData[y][x] == 9) // border
            {
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(1.0f, 1.0f, 1.0f));
                this->Board.push_back(obj);
            }
            else if (tileData[y][x] >= 1 && tileData[y][x] <= 8)	// non-solid; now determine its color based on tetromino data
            {            
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                GameObject obj(pos, size, ResourceManager::GetTexture("block"));
                if (tileData[y][x] == 1)
                    obj.Color = glm::vec3(0.87f, 0.83f, 0.91f);
                else if (tileData[y][x] == 2)
                    obj.Color = glm::vec3(0.99f, 0.87f, 0.93f);
                else if (tileData[y][x] == 3 || tileData[y][x] == 4)
                    obj.Color = glm::vec3(0.99f, 0.79f, 0.64f);
                else if (tileData[y][x] == 5 || tileData[y][x] == 6)
                    obj.Color = glm::vec3(0.77f, 0.92f, 1.0f);
                else if (tileData[y][x] == 7)
                    obj.Color = glm::vec3(0.85f, 1.0f, 0.92f);
                else if (tileData[y][x] == 8)
                    obj.Color = glm::vec3(0.44f, 0.33f, 0.65f);

                this->Board.push_back(obj);
            }
        }
    }
}