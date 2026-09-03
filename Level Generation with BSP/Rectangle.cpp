#include "Rectangle.h"
#include "Cell.h"

Rectangle::Rectangle() :
	cells{}, neighbers{}, colorNumber{}, mechanicNumber{}
{ }

void Rectangle::ExitCell(Cell* const cell)
{
	for (int i = 0; i < cells.size(); i++)
	{
		if (cells.at(i)->X() == cell->X() && cells.at(i)->Y() == cell->Y()) cells.erase(cells.begin() +  i);
	}
	sortCells();
}

void Rectangle::AddCell(Cell* cell)
{
	cells.push_back(cell);
	sortCells();
}

std::ostream& operator<<(std::ostream& Output, const Rectangle& rectangle)
{
	Output << rectangle.mechanicNumber;
	return Output;
}

void Rectangle::FindNeigbers()
{
	neighbers.clear();

	for (auto *cell : cells)
	{
		Rectangle* neighbor;
		neighbor = cell->LeftRectangle();
		if (neighbor != nullptr && !(neighbor == this))
		{
			if (neighbers.empty() || 
				std::find(neighbers.begin(), neighbers.end(), neighbor) == neighbers.end())
			{
				neighbers.push_back(neighbor);
			}
		}
		neighbor = cell->BottomRectangle();
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

Rectangle* Rectangle::GetNeighbor(int number) const
{
	return neighbers.at(number);
}

int Rectangle::Width() const
{
	return cells.at(cells.size() - 1)->X() - cells.at(0)->X() + 1;
}

int Rectangle::Height() const
{
	return cells.at(cells.size() - 1)->Y() - cells.at(0)->Y() + 1;
}

int Rectangle::CountNeighbors() const
{
	return neighbers.size();
}

int Rectangle::GetMechanic() const
{
	return mechanicNumber;
}

int Rectangle::GetColor() const
{
	return colorNumber;
}

bool Rectangle::operator<(const Rectangle& rectangle) const
{
	return Area() < rectangle.Area();
}

void Rectangle::sortCells()
{
	std::sort(cells.begin(), cells.end(),
		[](Cell* const cell1,Cell* const cell2)
		{
			return *cell1 < *cell2;
		});
}

bool Rectangle::operator==(const Rectangle& rectangle) const
{
	return mechanicNumber == rectangle.mechanicNumber;
}

void Rectangle::SetMechanic(int MechanicNumber) {
	mechanicNumber = MechanicNumber;
}

void Rectangle::SetColor(int ColorNumber) {
	colorNumber = ColorNumber;
}

Cell* const Rectangle::BaseCell() const
{
	return cells.at(0);
}

int Rectangle::Area() const {
	return cells.size();
}