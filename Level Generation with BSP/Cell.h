#pragma once

#include <iostream>
#include <string>

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

	std::string Layer0();
	std::string Layer1();
	std::string Layer2();

	bool operator<(const Cell& Neighbor) const;

	int X() const;
	int Y() const;

	Rectangle* LeftRectangle() const;

	Rectangle* TopRectangle() const;

	Rectangle* GetRectangle() const;

	void ChangeRectangle(Rectangle* const NewRectangle);
};