#include "Region.h"
#include "Cell.h"

Region::Region() :
	cells{}, neighbers{}, colorNumber{}, mechanicNumber{}
{ }

void Region::ExitCell(Cell* const cell)
{
	for (int i = 0; i < cells.size(); i++)
	{
		if (cells.at(i)->X() == cell->X() && cells.at(i)->Y() == cell->Y()) cells.erase(cells.begin() +  i);
	}
	sortCells();
}

void Region::AddCell(Cell* cell)
{
	cells.push_back(cell);
	sortCells();
}

std::ostream& operator<<(std::ostream& Output, const Region& region)
{
	Output << region.mechanicNumber;
	return Output;
}

void Region::FindNeigbers()
{
	neighbers.clear();

	for (auto *cell : cells)
	{
		Region* neighbor;
		neighbor = cell->LeftRegion();
		if (neighbor != nullptr && !(neighbor == this))
		{
			if (neighbers.empty() || 
				std::find(neighbers.begin(), neighbers.end(), neighbor) == neighbers.end())
			{
				neighbers.push_back(neighbor);
			}
		}
		neighbor = cell->BottomRegion();
		if (neighbor != nullptr && !(neighbor == this))
		{
			if (neighbers.empty() ||
				std::find(neighbers.begin(), neighbers.end(), neighbor) == neighbers.end())
			{
				neighbers.push_back(neighbor);
			}
		}
	}
}

Region* Region::GetNeighbor(int number) const
{
	return neighbers.at(number);
}

int Region::Width() const
{
	return cells.at(cells.size() - 1)->X() - cells.at(0)->X() + 1;
}

int Region::Height() const
{
	return cells.at(cells.size() - 1)->Y() - cells.at(0)->Y() + 1;
}

int Region::CountNeighbors() const
{
	return neighbers.size();
}

int Region::GetMechanic() const
{
	return mechanicNumber;
}

int Region::GetColor() const
{
	return colorNumber;
}

bool Region::operator<(const Region& region) const
{
	return Area() < region.Area();
}

void Region::sortCells()
{
	std::sort(cells.begin(), cells.end(),
		[](Cell* const cell1,Cell* const cell2)
		{
			return *cell1 < *cell2;
		});
}

bool Region::operator==(const Region& region) const
{
	return mechanicNumber == region.mechanicNumber;
}

void Region::SetMechanic(int MechanicNumber) {
	mechanicNumber = MechanicNumber;
}

void Region::SetColor(int ColorNumber) {
	colorNumber = ColorNumber;
}

Cell* const Region::BaseCell() const
{
	return cells.at(0);
}

int Region::Area() const {
	return cells.size();
}