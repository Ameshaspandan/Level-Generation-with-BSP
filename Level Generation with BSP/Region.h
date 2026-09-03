#pragma once

#include <vector>

#include "Rectangle.h"

class Region
{
	using Convex = std::vector<Rectangle*>;
private:
	Convex convex;
public:
	Region();
	~Region();

	const Convex& GetConvex() const;

	void AddRecangle(Rectangle*);
	bool RemoveRecangle(Rectangle*);
	void SetMechanic(int Mechanic);

	int Area;
};

