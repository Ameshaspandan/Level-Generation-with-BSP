#include "Region.h"

#include <algorithm>
#include <cmath>
#include "Cell.h"

using Convex = std::vector<Rectangle*>;

Region::Region() : convex{}, area{}
{
}

Region::~Region()
{
}

const Convex& Region::GetConvex() const
{
	return convex;
}

void Region::AddRecangle(Rectangle* rectangle)
{
	convex.push_back(rectangle);
	area += rectangle->Area();
}

bool Region::RemoveRecangle(Rectangle* rectangle)
{
	auto search = std::find(convex.begin(), convex.end(), rectangle);
	if (search != convex.end())
	{
		convex.erase(search);
		area -= rectangle->Area();
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

int Region::Size()
{
	return convex.size();
}

bool Region::isRectangle(std::vector<Cell*> cells)
{
    if (cells.empty())
        return false;

    std::sort(
        cells.begin(),
        cells.end(),
        [](Cell* a, Cell* b)
        {
            return *a < *b;
        }
    );

    int minX = cells.front()->X();
    int minY = cells.front()->Y();

    int maxX = minX;
    int maxY = minY;

    for (auto* cell : cells)
    {
        maxX = std::max(maxX, cell->X());
        maxY = std::max(maxY, cell->Y());
    }

    int width = maxX - minX + 1;
    int height = maxY - minY + 1;

    if (width * height != static_cast<int>(cells.size()))
        return false;

    int index = 0;

    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x++)
        {
            Cell* cell = cells.at(index);

            if (cell->X() != x ||
                cell->Y() != y)
            {
                return false;
            }

            index++;
        }
    }

    return true;
}

bool Region::IsEvenRectangle()
{
    std::vector<Cell*> regionCells;

    for (auto* rectangle : GetConvex())
    {
        for (auto* cell : *rectangle->GetCells())
        {
            regionCells.push_back(cell);
        }
    }

    if (!isRectangle(regionCells))
        return false;

    std::sort(
        regionCells.begin(),
        regionCells.end(),
        [](Cell* a, Cell* b)
        {
            return *a < *b;
        }
    );

    int minX = regionCells.front()->X();
    int minY = regionCells.front()->Y();

    int maxX = minX;
    int maxY = minY;

    for (auto* cell : regionCells)
    {
        maxX = std::max(maxX, cell->X());
        maxY = std::max(maxY, cell->Y());
    }

    int width = maxX - minX + 1;
    int height = maxY - minY + 1;

    return
        width % 2 == 0 &&
        height % 2 == 0;
}