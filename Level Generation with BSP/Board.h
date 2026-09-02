#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <random>

#include "Region.h"
#include "Cell.h"

class Board
{
private:
	std::vector<Region*> regions;
	std::vector<Cell*> cells;

	std::mt19937 rng;

	const int tryLimit = 100;

	bool regionSplit(Region*);
	void mesh(int RectangleCount);
	void paint(int MaxSize);

	void regionSort()
	{
		std::sort(regions.begin(), regions.end(),
			[](Region* const region1, Region* const region2)
			{
				return (*region1 < *region2);
			});
	}

	Cell* const getCell(int X, int Y) const;

	int width, height;

	int minWidth, minHeight;

	int randomInt(int min, int max)
	{
		std::uniform_int_distribution<int> dist(min, max);
		return dist(rng);
	}

	void resetRegion()
	{
		Region* newRegion = new Region();

		for (int n = 0; n < cells.size(); n++)
		{
			cells.at(n)->ChangeRegion(newRegion);
		}

		for (int n = 0; n < regions.size(); n++)
		{
			delete regions.at(n);
		}
		regions.clear();

		regions.push_back(newRegion);
	}

public:
	Board(int Width, int Height);
	~Board();

	void LevelGenerate(int RegionCount, int ColorCount, int MaxArea);

	friend std::ostream& operator<<(std::ostream&, const Board&);
};