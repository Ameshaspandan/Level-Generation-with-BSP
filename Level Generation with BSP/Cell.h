#pragma once

#include <iostream>

class Rectangle;

class Cell
{
private:
	int  x, y;

	Rectangle* rectangle;

	Cell* bottom;
	Cell* left;
public:
	Cell(int X, int Y, Rectangle* Rectangle, Cell* Left = nullptr, Cell* Bottom = nullptr);

	friend std::ostream& operator<<(std::ostream&, const Cell&);

	bool operator<(const Cell& neighbor) const;

	int X() const;
	int Y() const;

	Rectangle* LeftRectangle() const;

	Rectangle* BottomRectangle() const;

	Rectangle* GetRectangle() const;

	void ChangeRectangle(Rectangle* const NewRectangle);
};