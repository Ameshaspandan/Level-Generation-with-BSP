#include "Region.h"

#include <algorithm>>

using Convex = std::vector<Rectangle*>;

Region::Region() : convex{}, Area{}
{
}

Region::~Region()
{
	for (auto* rectangle : convex)
	{
		delete rectangle;
	}
}

const Convex& Region::GetConvex() const
{
	return convex;
}

void Region::AddRecangle(Rectangle* rectangle)
{
	convex.push_back(rectangle);
	Area += rectangle->Area();
}

bool Region::RemoveRecangle(Rectangle* rectangle)
{
	auto search = std::find(convex.begin(), convex.end(), rectangle);
	if (search != convex.end())
	{
		convex.erase(search);
		Area -= rectangle->Area();
		return true;
	}
	return false;
}

void Region::SetMechanic(int Mechanic)
{
	for (auto* rectangle : convex)
	{
		rectangle->SetMechanic(Mechanic);
	}
}