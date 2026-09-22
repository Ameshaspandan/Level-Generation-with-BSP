#pragma once

#include <iostream>

class Rectangle;

class Cell
{
private:
	int  x, y;

	Rectangle* rectangle;

	Cell* top;
	Cell* left;
public:
	Cell(int X, int Y, Rectangle* Rectangle, Cell* Left = nullptr, Cell* Top = nullptr);

	int Layer0();
	int Layer1();
	int Layer2();

	bool operator<(const Cell& Neighbor) const;

	int X() const;
	int Y() const;

	Rectangle* LeftRectangle() const;

	Rectangle* TopRectangle() const;

	Rectangle* GetRectangle() const;

	void ChangeRectangle(Rectangle* const NewRectangle);
};