#pragma once

#include <iostream>

class Region;

class Cell
{
private:
	int  x, y;

	Region* region;

	Cell* bottom;
	Cell* left;
public:
	Cell(int X, int Y, Region* Region, Cell* Left = nullptr, Cell* Bottom = nullptr);

	friend std::ostream& operator<<(std::ostream&, const Cell&);

	bool operator<(const Cell& neighbor) const;

	int X() const;
	int Y() const;

	Region* LeftRegion() const;

	Region* BottomRegion() const;

	void ChangeRegion(Region* const NewRegion);
};