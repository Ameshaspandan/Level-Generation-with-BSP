#pragma once

#include <vector>

#include "Rectangle.h"

class Region
{
	using Convex = std::vector<Rectangle*>;

	int area;
private:
	Convex convex;

	bool isRectangle(std::vector<Cell*> cells);
public:
	Region();
	~Region();

	const Convex& GetConvex() const;

	void AddRectangle(Rectangle*);
	bool RemoveRecangle(Rectangle*);
	void SetMechanicLayer0(int Mechanic);
	void SetMechanicLayer1(int Mechanic);
	void SetMechanicLayer2(int Mechanic);
	int GetColor() { return convex.at(0)->GetColor(); };
	int GetMechanicLayer0() { return convex.at(0)->GetMechanicLayer0(); };
	int GetMechanicLayer1() { return convex.at(0)->GetMechanicLayer1(); };
	int GetMechanicLayer2() { return convex.at(0)->GetMechanicLayer2(); };

	int Size();

	int Area() { return area; };

	bool IsEvenRectangle();
};

