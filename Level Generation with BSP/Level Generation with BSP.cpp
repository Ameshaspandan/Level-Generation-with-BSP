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

    std::ofstream file("layouts.csv");

    if (!file.is_open())
    {
        std::cerr
            << "Failed to open CSV file.\n";

        return 1;
    }

    for (int i = 0; i < 1000; i++)
    {
        file << "Layout " << i + 1 << "\n";
        board.LevelGenerate(7, 5);
        file << board;
    }

    file.close();
    return 0;
}