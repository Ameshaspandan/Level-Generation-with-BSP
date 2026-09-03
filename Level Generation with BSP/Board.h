#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include <random>

#include "Rectangle.h"
#include "Cell.h"

class Board
{
private:
	std::vector<Rectangle*> rectangles;
	std::vector<Cell*> cells;

	std::mt19937 rng;

	const int tryLimit = 100;
	const int smallestM1Area = 16;
	const int largetM1Area = 20;

	bool rectangleSplit(Rectangle*);
	void mesh(int RectangleCount);
	void paint();

	std::vector<std::vector<Rectangle*>*> colorRectangles; //is a hack

	void rectangleSort()
	{
		std::sort(rectangles.begin(), rectangles.end(),
			[](Rectangle* const rectangle1, Rectangle* const rectangle2)
			{
				return (*rectangle1 < *rectangle2);
			});
	}

	void findColorRectangle();
	void setColorMechanic(std::vector<Rectangle*>* ColorRectangle, int Mechanic);
	int getColorColorRectangle(std::vector<Rectangle*>* ColorRectangle);
	bool touchesBoardEdge(Rectangle* Rectangle);

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
		Rectangle* newRectangle = new Rectangle();

		for (int n = 0; n < cells.size(); n++)
		{
			cells.at(n)->ChangeRectangle(newRectangle);
		}

		for (int n = 0; n < rectangles.size(); n++)
		{
			delete rectangles.at(n);
		}
		rectangles.clear();

		rectangles.push_back(newRectangle);
	}

public:
	Board(int Width, int Height);
	~Board();

	void LevelGenerate(int RectangleCount, int ColorCount);

	friend std::ostream& operator<<(std::ostream&, const Board&);
};