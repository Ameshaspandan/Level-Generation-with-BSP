#include "Cell.h"
#include "Rectangle.h"

Cell::Cell(int X, int Y, Rectangle* Rectangle, Cell* Left, Cell* Bottom) :
	x{ X }, y{ Y }, left{ Left }, bottom{ Bottom }
{
	rectangle = Rectangle;
	rectangle->AddCell(this);
}

std::ostream& operator<<(std::ostream& Output, const Cell& cell)
{
	Output << *(cell.rectangle);
	return Output;
}

bool Cell::operator<(const Cell& neighbor) const
{
	if (y != neighbor.y) return y < neighbor.y;
	return x < neighbor.x;
}

void Cell::ChangeRectangle(Rectangle* const NewRectangle) {
	rectangle->ExitCell(this);
	rectangle = NewRectangle;
	rectangle->AddCell(this);
}

Rectangle* Cell::BottomRectangle() const
{
	return bottom == nullptr ? nullptr : bottom->rectangle;
}

Rectangle* Cell::LeftRectangle() const
{
	return left == nullptr ? nullptr : left->rectangle;
}

Rectangle* Cell::GetRectangle() const 
{
	return rectangle;
}

int Cell::X() const { return x; }

int Cell::Y() const { return y; }