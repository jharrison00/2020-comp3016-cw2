#pragma once
#include <vector>
#include <array>
#ifndef TETROMINOS_H
#define TETROMINOS_H

// Class to hold all types of tetrominos with getter and setter for arrays
class Tetrominos 
{
public:
	std::vector < std::array<int, 16>> Tetros;

	Tetrominos();
	std::array<int, 16> GetTetromino(int tetIndex);
};

#endif

