#include "Board.h"
#include <iomanip>
#include <random>

Board::Board(int Width, int Height) :
    height{ Height }, width{ Width }, regions{}, cells{}, minHeight{1}, minWidth{1}, rng{ std::random_device{}() }
{
    Region* baseRegion = new Region();
    regions.push_back(baseRegion);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			Cell* newCell = new Cell(x, y, regions.at(0), x > 0 ? getCell(x - 1, y) : nullptr, y > 0 ? getCell(x, y - 1) : nullptr);
            cells.push_back(newCell);
		}
	}
}

Board::~Board()
{
    for (int n = 0; n < regions.size(); n++)
    {
        delete regions.at(n);
    }
    for (int n = 0; n < cells.size(); n++)
    {
        delete cells.at(n);
    }
}

Cell* const Board::getCell(int X, int Y) const
{
	return cells.at(X + Y * width);
}

std::ostream& operator<<(std::ostream& Output, const Board& board)
{
    for (int y = 0; y < board.height; y++)
    {
        for (int x = 0; x < board.width; x++)
        {
            Output
                << std::setw(2)
                << *board.getCell(x, y)
                << ",";
        }

        for (int x = board.width - 2; x >= 0; x--)
        {
            Output
                << std::setw(2)
                << *board.getCell(x, y)
                << ",";
        }
        Output << "\n";
    }
	return Output;
}

bool Board::regionSplit(Region* motherRegion)
{
    int motherWidth = motherRegion->Width();
    int motherHeight = motherRegion->Height();

    bool canSplitVertical =
        motherWidth >= 2 * minWidth;

    bool canSplitHorizontal =
        motherHeight >= 2 * minHeight;

    if (!canSplitVertical && !canSplitHorizontal)
        return false;

    bool splitVertical;

    if (canSplitVertical && canSplitHorizontal)
    {

        splitVertical = randomInt(0, 1) == 0;
    }
    else
    {
        splitVertical = canSplitVertical;
    }

    Region* daughterRegion = new Region();

    regions.push_back(daughterRegion);

    if (splitVertical)
    {
        // Vertical split
        int daughterWidth = randomInt(minWidth, motherWidth - minWidth);

        for (int x = motherWidth - daughterWidth; x < motherWidth; x++)
        {
            for (int y = 0; y < motherHeight; y++)
            {
                getCell(x + motherRegion->BaseCell()->X(), y + motherRegion->BaseCell()->Y())->ChangeRegion(daughterRegion);
            }
        }
    }
    else
    {
        // Horizontal split
        int daughterHeight =
            randomInt(minHeight, motherHeight - minHeight);

        for (int x = 0; x < motherWidth; x++)
        {
            for (int y = motherHeight - daughterHeight; y < motherHeight; y++)
            {
                getCell(x + motherRegion->BaseCell()->X(), y + motherRegion->BaseCell()->Y())->ChangeRegion(daughterRegion);
            }
        }
    }
    return true;
}

void Board::mesh(int ColorCount)
{
    regionSort();
    for (int i = 0; i < regions.size(); i++)
    {
        regions.at(i)->FindNeigbers();
    }

    for (int i = 0; i < regions.size(); i++)
    {
        regions.at(i)->SetMechanic(i);
        regions.at(i)->SetColor(i);
    }

    for (int n = 0; n < regions.size() - ColorCount; n++)
    {
        std::vector<Region*> validRegions;

        // Find regions that have at least one neighbor
        // with a different mechanic.
        for (Region* region : regions)
        {
            for (int i = 0; i < region->CountNeighbors(); i++)
            {
                Region* neighbor = region->GetNeighbor(i);

                if (neighbor->GetMechanic() != region->GetMechanic())
                {
                    validRegions.push_back(region);
                    break;
                }
                //Ali push
            }
        }

        if (validRegions.empty())
            break;

        // Choose a base region.
        int baseIndex = randomInt(0, validRegions.size() - 1);
        Region* baseRegion = validRegions.at(baseIndex);

        // Find neighbors with a different mechanic.
        std::vector<Region*> differentNeighbors;

        for (int i = 0; i < baseRegion->CountNeighbors(); i++)
        {
            Region* neighbor = baseRegion->GetNeighbor(i);

            if (neighbor->GetMechanic() != baseRegion->GetMechanic())
            {
                differentNeighbors.push_back(neighbor);
            }
        }

        if (differentNeighbors.empty())
            continue;

        // Choose one different-mechanic neighbor.
        int neighborIndex =
            randomInt(0, differentNeighbors.size() - 1);

        Region* neighbor = differentNeighbors.at(neighborIndex);

        int baseMechanic = baseRegion->GetMechanic();
        int neighborMechanic = neighbor->GetMechanic();

        // Merge the entire neighbor mechanic group
        // into the base mechanic group.
        for (Region* region : regions)
        {
            if (region->GetMechanic() == neighborMechanic)
            {
                region->SetMechanic(baseMechanic);
            }
        }
    }
}

void Board::paint(int MaxSize)
{

}

void Board::LevelGenerate(int RegionCount, int ColorCount, int MaxArea)
{
    resetRegion();

    for (int n = 0; n < RegionCount - 1; n++)
    {
        for (int i = 0; i < tryLimit; i++)
        {
            if (regionSplit(regions.at(randomInt(0, regions.size() - 1)))) break;
        }
    }

    mesh(ColorCount);
}