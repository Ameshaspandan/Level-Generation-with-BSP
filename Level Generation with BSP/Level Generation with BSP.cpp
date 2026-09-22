#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <fstream>

#include "Board.h"

int main()
{
    Board board{9, 9};

    int levelCount = 100;

    std::ofstream file("layouts.csv");

    if (!file.is_open())
    {
        std::cerr
            << "Failed to open CSV file.\n";

        return 1;
    }

    for (int i = 0; i < levelCount; i++)
    {
        file << "Layout " << i + 1 << "\n";
        board.LevelGenerate(13, 5, true, true, true, false, false, true, true); //LevelGenerate(int RectangleCount, int ColorCount, bool M0, bool M2, bool M3, bool M4, bool M5)
        file << board;
    }

    file.close();
    return 0;
}