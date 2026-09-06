#include "Rectangle.h"
#include "Cell.h"
#include "Utility.h"

Rectangle::Rectangle(std::vector<Cell*>* Cells) :
	cells{Cells}, neighbers{}, colorNumber{}, mechanicNumber{}
{
	sortCells();
	recalculateSize();

	for (auto* cell : *cells)
	{
		cell->ChangeRectangle(this);
	}
}

Rectangle::Rectangle(int Width, int Height) :
	neighbers{}, colorNumber{}, mechanicNumber{}, width{Width}, height{Height}
{
	cells = new std::vector<Cell*>();
	for (int y = 0; y < Height; y++)
	{
		for (int x = 0; x < Width; x++)
		{
			Cell* newCell = new Cell(x, y, this, x > 0 ? getCell(x - 1, y) : nullptr, y > 0 ? getCell(x, y - 1) : nullptr);
			cells->push_back(newCell);
		}
	}
}

Rectangle::~Rectangle()
{
	for (auto* cell : *cells)
	{
		delete cell;
	}
	delete cells;
}

void Rectangle::ExitCell(Cell* const cell)
{
	for (int i = 0; i < cells->size(); i++)
	{
		if (cells->at(i)->X() == cell->X() && cells->at(i)->Y() == cell->Y()) cells->erase(cells->begin() +  i);
	}
	sortCells();
}

void Rectangle::AddCell(Cell* cell)
{
	cells->push_back(cell);
	sortCells();
}

std::ostream& operator<<(std::ostream& Output, const Rectangle& rectangle)
{
	Output << rectangle.mechanicNumber;
	return Output;
}

void Rectangle::findNeigbers()
{
	neighbers.clear();

	for (auto *cell : *cells)
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

void Rectangle::recalculateSize()
{
	width = cells->at(cells->size() - 1)->X() - cells->at(0)->X() + 1;
	height = cells->at(cells->size() - 1)->Y() - cells->at(0)->Y() + 1;
}

Rectangle* Rectangle::GetNeighbor(int number) const
{
	return neighbers.at(number);
}

int Rectangle::Width() const
{
	return width;
}

int Rectangle::Height() const
{
	return height;
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
	std::sort(cells->begin(), cells->end(),
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
	return cells->at(0);
}

int Rectangle::Area() const {
	return cells->size();
}

Cell* Rectangle::getCell(int X, int Y)
{
	return cells->at(X + Y * width);
}

const std::vector<Cell*>* Rectangle::getCells() const
{
	return cells;
}

Rectangle* Rectangle::Split()
{
	Random rng{};

	bool canSplitVertical =
		width >= 2 * minWidth;

	bool canSplitHorizontal =
		height >= 2 * minHeight;

	if (!canSplitVertical && !canSplitHorizontal)
		return nullptr;

	bool splitVertical;

	if (canSplitVertical && canSplitHorizontal)
	{

		splitVertical = rng.randomInt(0, 1) == 0;
	}
	else
	{
		splitVertical = canSplitVertical;
	}

	Rectangle* daughterRectangle;
	std::vector<Cell*>* daughterCells = new std::vector<Cell*>();

	if (splitVertical)
	{
		// Vertical split
		int daughterWidth = rng.randomInt(minWidth, width - minWidth);

		for (int y = 0; y < height; y++)
		{
			for (int x = width - daughterWidth; x < width; x++)
			{
				auto selectedCell = getCell(x, y);
				daughterCells->push_back(selectedCell);
			}
		}

		for (auto* cell : *daughterCells)
		{
			auto it = std::find(cells->begin(), cells->end(), cell);
			cells->erase(it);
		}
	}
	else
	{
		// Horizontal split
		int daughterHeight = rng.randomInt(minHeight, height - minHeight);

		for (int x = 0; x < width; x++)
		{
			for (int y = height - daughterHeight; y < height; y++)
			{
				auto selectedCell = getCell(x, y);
				daughterCells->push_back(selectedCell);
			}
		}

		for (auto* cell : *daughterCells)
		{
			cells->erase(std::find(cells->begin(), cells->end(), cell));
		}
	}

	sortCells();
	recalculateSize();

	daughterRectangle = new Rectangle(daughterCells);

	findNeigbers();
	daughterRectangle->findNeigbers();
	return daughterRectangle;
}
