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

	void AddRecangle(Rectangle*);
	bool RemoveRecangle(Rectangle*);
	void SetMechanic(int Mechanic);
	int GetColor() { return convex.at(0)->GetColor(); };
	int GetMechanic() { return convex.at(0)->GetMechanic(); };

	int Size();

	int Area() { return area; };

	bool IsEvenRectangle();
};

