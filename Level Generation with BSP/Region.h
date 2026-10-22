#pragma once

#include <vector>
#include <string>

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
	void SetMechanicLayer0(std::string Mechanic);
	void SetMechanicLayer1(std::string Mechanic);
	void SetMechanicLayer2(std::string Mechanic);
	int GetColor() { return convex.at(0)->GetColor(); };
	std::string GetMechanicLayer0() { return convex.at(0)->GetMechanicLayer0(); };
	std::string GetMechanicLayer1() { return convex.at(0)->GetMechanicLayer1(); };
	std::string GetMechanicLayer2() { return convex.at(0)->GetMechanicLayer2(); };

	int Size();

	int Area() { return area; };

	bool IsEvenRectangle();
};

