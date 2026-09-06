#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <random>

#include "Rectangle.h"
#include "Cell.h"
#include "Region.h"

class Board
{
private:
	std::vector<Region*> regions;
	std::vector<Rectangle*> rectangles;
	std::vector<Cell*> cells;

	std::mt19937 rng;

	const int tryLimit = 100;
	const int smallestM1Area = 16;
	const int largetM1Area = 20;

	void mesh(int RectangleCount);
	bool paint(int M1Area, bool M2, bool M3, bool M4, bool M5);

	void rectangleSort()
	{
		std::sort(rectangles.begin(), rectangles.end(),
			[](Rectangle* const rectangle1, Rectangle* const rectangle2)
			{
				return (*rectangle1 < *rectangle2);
			});
	}

	void findRegion();
	void setColorMechanic(std::vector<Rectangle*>* ColorRectangle, int Mechanic);
	int getColorColorRectangle(std::vector<Rectangle*>* ColorRectangle);
	bool touchesBoardEdge(Rectangle* Rectangle);

	bool isM2Candidate(Region* region);
	bool isM4Candidate(Region* region);
	bool isM3Candidate(Region* region);

	Cell* const getCell(int X, int Y) const;

	int width, height;

	int minWidth, minHeight;

	int randomInt(int min, int max)
	{
		std::uniform_int_distribution<int> dist(min, max);
		return dist(rng);
	}

	void resetRectangle()
	{

		for (int i = 0; i < regions.size(); i++)
		{
			delete regions.at(i);
		}
		regions.clear();

		Rectangle* newRectangle = new Rectangle(width, height);

		for (int n = 0; n < rectangles.size(); n++)
		{
			delete rectangles.at(n);
		}

		rectangles.clear();

		rectangles.push_back(newRectangle);

		cells = *newRectangle->getCells();
	}

public:
	Board(int Width, int Height);
	~Board();

	void LevelGenerate(int RectangleCount, int ColorCount, int M1Area, bool M2, bool M3, bool M4, bool M5);

	void TestAllRectangleNeighbors();

	friend std::ostream& operator<<(std::ostream&, const Board&);
};