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
	

	const int smallestM0Area = 5;
	const int smallestM1Area = 15;
	const int largestM1Area = 25;
	const int smallestM2Area = 5;
	const int smallestM3Area = 5;
	const int smallestM4Area = 5;
	const int smallestM5Area = 4;

	const int smallestMdArea = 20;
	const int smallestMuArea = 20;
	const int largestMdArea = 50;
	const int largestMuArea = 50;

	void mesh(int RectangleCount);
	bool paint(bool M0, bool M2, bool M3, bool M4, bool M5, bool Md, bool Mu);

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
	bool isMuCandidate(Region* region);
	bool isMdCandidate(Region* region);

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

	void LevelGenerate(int RectangleCount, int ColorCount, bool M0, bool M2, bool M3, bool M4, bool M5, bool Md, bool Mu);

	void TestAllRectangleNeighbors();

	friend std::ostream& operator<<(std::ostream&, const Board&);
};