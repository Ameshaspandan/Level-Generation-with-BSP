#include "Cell.h"
#include "Region.h"

Cell::Cell(int X, int Y, Region* Region, Cell* Left, Cell* Bottom) :
	x{ X }, y{ Y }, left{ Left }, bottom{ Bottom }
{
	region = Region;
	region->AddCell(this);
}

std::ostream& operator<<(std::ostream& Output, const Cell& cell)
{
	Output << *(cell.region);
	return Output;
}

bool Cell::operator<(const Cell& neighbor) const
{
	if (y != neighbor.y) return y < neighbor.y;
	return x < neighbor.x;
}

void Cell::ChangeRegion(Region* const NewRegion) {
	region->ExitCell(this);
	region = NewRegion;
	region->AddCell(this);
}

Region* Cell::BottomRegion() const
{
	return bottom == nullptr ? nullptr : bottom->region;
}

Region* Cell::LeftRegion() const
{
	return left == nullptr ? nullptr : left->region;
}

int Cell::X() const { return x; }

int Cell::Y() const { return y; }