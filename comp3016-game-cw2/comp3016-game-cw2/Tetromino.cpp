#include "Tetromino.h"
#include "ResourceManager.h"

Tetromino::Tetromino()
	: TetrominoIndex(0), RotationIndex(0), OffsetX(5), OffsetY(-2) 
{
	this->Tetro = GetTetromino(TetrominoIndex);
}

Tetromino::Tetromino(int tetrominoIndex, int rotationIndex, unsigned int levelWidth, unsigned int levelHeight, unsigned int boardWidth, unsigned int boardHeight, bool isChaos)
	: TetrominoIndex(tetrominoIndex), RotationIndex(rotationIndex), OffsetX(5), OffsetY(-2), Chaos(isChaos)
{
	this->Tetro = GetTetromino(TetrominoIndex);
	CreateBlocks(levelWidth, levelHeight, boardWidth, boardHeight, isChaos);
}

void Tetromino::Draw(SpriteRenderer& renderer)
{
		for (GameObject& block : this->Blocks)
		{
			block.Draw(renderer);
		}
}

void Tetromino::CreateBlocks(unsigned int levelWidth, unsigned int levelHeight, unsigned int boardWidth, unsigned int boardHeight, bool isChaos)
{
	// clear blocks
	this->Blocks.clear();
	// create temp tetro array
	std::array<int, 16> tets;
	tets = this->Tetro;
	float unit_width = levelWidth / static_cast<float>(boardWidth), unit_height = levelHeight / boardHeight;

	// set GameObject values for each block of tetromino
	for (size_t i = 0; i < 16; i++)
	{
		if (tets[i] != 0)
		{
			// get x and y based off position in tetromino array and global position
			int x = (i % 4) + OffsetX;	
			int y = (i / 4) + OffsetY;
			glm::vec2 pos(unit_width * x, unit_height * y);
			glm::vec2 size(unit_width, unit_height);
			// set colour
			GameObject obj(pos, size, ResourceManager::GetTexture("block"));
			if (isChaos)
				obj.Color = glm::vec3(0.44f, 0.33f, 0.65f);
			else if (this->TetrominoIndex == 0)
				obj.Color = glm::vec3(0.87f, 0.83f, 0.91f);
			else if (this->TetrominoIndex == 1)
				obj.Color = glm::vec3(0.99f, 0.87f, 0.93f);
			else if (this->TetrominoIndex == 2 || this->TetrominoIndex == 3)
				obj.Color = glm::vec3(0.99f, 0.79f, 0.64f);
			else if (this->TetrominoIndex == 4 || this->TetrominoIndex == 5)
				obj.Color = glm::vec3(0.77f, 0.92f, 1.0f);
			else if (this->TetrominoIndex == 6)
				obj.Color = glm::vec3(0.85f, 1.0f, 0.92f);

			this->Blocks.push_back(obj);
		}
	}
	this->Blocks;
}

void Tetromino::CreateNextBlock()
{
	//find minX and minY
	float x, y, minX = 1000.0f, minY = 1000.0f;

	for (size_t i = 0; i < Blocks.size(); i++)
	{
		x = Blocks[i].Position[0];
		y = Blocks[i].Position[1];
		if (x < minX)
			minX = 640 - x;
		if (y < minY)
			minY = 285 - y;
	}

	//Change pos based on minX
	for (size_t i = 0; i < Blocks.size(); i++)
	{
		Blocks[i].Position = glm::vec2(Blocks[i].Position[0] + minX, Blocks[i].Position[1] + minY);
	}
}

void Tetromino::SetRotation(int rotationIndex)
{
	this->RotationIndex = rotationIndex;

	// check if rotate needs to be 3x3 or 4x4 via index
	if (this->TetrominoIndex <= 1)
	{
		Rotate4x4(rotationIndex);
	}
	else
	{
		Rotate3x3(rotationIndex);
	}	
}

void Tetromino::Rotate3x3(int rotationIndex)
{
	// convert 4x4 to 3x3 prior to rotate
	std::array<int, 16> normalTetro = GetTetromino(this->TetrominoIndex);
	std::array<int, 9> preRotatedTetro;
	std::array<int, 9> rotatedTetro;
	int gridPos = 0;
	for (size_t y = 0; y < 4; y++)
	{
		for (size_t x = 0; x < 4; x++)
		{
			if (y == 0 || x == 0){}
			else
			{
				preRotatedTetro[gridPos] = normalTetro[y * 4 + x];
				gridPos++;
			}
		}
	}

	// rotate 3x3 grid
	switch (abs(rotationIndex) % 4)
	{
	case 0:	// 0 degrees
		rotatedTetro = preRotatedTetro;
		break;
	case 1:	// 90 degrees
		for (size_t y = 0; y < 3; y++)
		{
			for (size_t x = 0; x < 3; x++)
			{
				rotatedTetro[y * 3 + x] = preRotatedTetro[6 + y - (x * 3)];
			}
		}
		break;
	case 2:	// 180 degrees
		for (size_t y = 0; y < 3; y++)
		{
			for (size_t x = 0; x < 3; x++)
			{
				rotatedTetro[y * 3 + x] = preRotatedTetro[8 - (y * 3) - x];
			}
		}
		break;
	case 3:	// 270 degrees
		for (size_t y = 0; y < 3; y++)
		{
			for (size_t x = 0; x < 3; x++)
			{
				rotatedTetro[y * 3 + x] = preRotatedTetro[2 - y + (x * 3)];
			}
		}
		break;
	}

	// convert 3x3 back to 4x4
	std::array<int, 16> postRotatedTetro;
	for (size_t i = 0; i < 4; i++)
	{
		postRotatedTetro[i] = 0;
		postRotatedTetro[i * 4] = 0;
	}

	for (size_t y = 0; y < 3; y++)
	{
		for (size_t x = 0; x < 3; x++)
		{
			postRotatedTetro[((y + 1) * 4) + 1 + x] = rotatedTetro[y * 3 + x];
		}
	}
	this->Tetro = postRotatedTetro;
}

void Tetromino::Rotate4x4(int rotationIndex)
{
	std::array<int, 16> normalTetro = GetTetromino(this->TetrominoIndex);
	std::array<int, 16> rotatedTetro;
	switch (abs(rotationIndex) % 4)
	{
	case 0:	// 0 degrees
		this->Tetro = normalTetro;
		break;
	case 1:	// 90 degrees
		for (size_t y = 0; y < 4; y++)
		{
			for (size_t x = 0; x < 4; x++)
			{
				rotatedTetro[y * 4 + x] = normalTetro[12 + y - (x * 4)];
			}
		}
		this->Tetro = rotatedTetro;
		break;
	case 2:	// 180 degrees
		for (size_t y = 0; y < 4; y++)
		{
			for (size_t x = 0; x < 4; x++)
			{
				rotatedTetro[y * 4 + x] = normalTetro[15 - (y * 4) - x];
			}
		}
		this->Tetro = rotatedTetro;
		break;
	case 3:	// 270 degrees
		for (size_t y = 0; y < 4; y++)
		{
			for (size_t x = 0; x < 4; x++)
			{
				rotatedTetro[y * 4 + x] = normalTetro[3 - y + (x * 4)];
			}
		}
		this->Tetro = rotatedTetro;
		break;
	}
}
