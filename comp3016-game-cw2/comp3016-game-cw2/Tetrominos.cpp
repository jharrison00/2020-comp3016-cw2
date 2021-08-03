#include "Tetrominos.h"

Tetrominos::Tetrominos()
{
    std::array<int, 16> ITet = {
       0,0,1,0,
       0,0,1,0,
       0,0,1,0,
       0,0,1,0
    };
    std::array<int, 16> OTet = {
        0,0,0,0,
        0,1,1,0,
        0,1,1,0,
        0,0,0,0
    };
    std::array<int, 16> LTet = {
        0,0,0,0,
        0,0,1,1,
        0,0,1,0,
        0,0,1,0
    };
    std::array<int, 16> LTet2 = {
        0,0,0,0,
        0,1,1,0,
        0,0,1,0,
        0,0,1,0
    };
    std::array<int, 16> ZTet = {
        0,0,0,0,
        0,0,0,1,
        0,0,1,1,
        0,0,1,0
    };
    std::array<int, 16> ZTet2 = {
        0,0,0,0,
        0,0,1,0,
        0,0,1,1,
        0,0,0,1
    };
    std::array<int, 16> TTet = {
        0,0,0,0,
        0,0,1,0,
        0,1,1,1,
        0,0,0,0
    };
    Tetros.push_back(ITet);
    Tetros.push_back(OTet);
    Tetros.push_back(LTet);
    Tetros.push_back(LTet2);
    Tetros.push_back(ZTet);
    Tetros.push_back(ZTet2);
    Tetros.push_back(TTet);
}

std::array<int, 16> Tetrominos::GetTetromino(int tetIndex)
{
    return Tetros.at(tetIndex);
}
