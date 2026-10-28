#include "Rectangle.h"
#include "Cell.h"
#include "Utility.h"

Rectangle::Rectangle(std::vector<Cell*>* Cells) :
	cells{ Cells }, neighbers{}, colorNumber{}, mechanicLayer0{}, mechanicLayer1{}, mechanicLayer2{}, region{}
{
	sortCells();
	recalculateSize();

	for (auto* cell : *cells)
	{
		cell->ChangeRectangle(this);
	}
}

Rectangle::Rectangle(int Width, int Height) :
    neighbers{}, colorNumber{}, mechanicLayer0{}, mechanicLayer1{}, mechanicLayer2{}, width{ Width }, height{ Height }, region{}
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
	auto it = std::find(cells->begin(), cells->end(), cell);
	if (it == cells->end()) return;
	cells->erase(it);
	cell->ChangeRectangle(nullptr);
	sortCells();
	recalculateSize();
	FindNeigbers();
}

void Rectangle::AddCell(Cell* cell)
{
	if (!cell) throw std::invalid_argument("Cannot add a null cell.");
	if (std::find(cells->begin(), cells->end(), cell) != cells->end()) return;
	if (cell->GetRectangle() != nullptr)
		throw std::invalid_argument("Detach the cell from its rectangle before adding it.");
	for (auto* existing : *cells)
		if (existing->X() == cell->X() && existing->Y() == cell->Y())
			throw std::invalid_argument("Duplicate cell coordinates.");
	cells->push_back(cell);
	cell->ChangeRectangle(this);
	sortCells();
	recalculateSize();
	FindNeigbers();
}

std::ostream& Rectangle::Layer0(std::ostream& Output)
{
	Output << mechanicLayer0;
	return Output;
}

std::ostream& Rectangle::Layer1(std::ostream& Output)
{
    Output << mechanicLayer1;
    return Output;
}

std::ostream& Rectangle::Layer2(std::ostream& Output)
{
    Output << mechanicLayer2;
    return Output;
}

void Rectangle::FindNeigbers()
{
	neighbers.clear();
	leftNeighbors.clear();
	topNeighbors.clear();

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
				leftNeighbors.push_back(neighbor);
			}
		}
		neighbor = cell->TopRectangle();
		if (neighbor != nullptr && !(neighbor == this))
		{
			if (neighbers.empty() ||
				std::find(neighbers.begin(), neighbers.end(), neighbor) == neighbers.end())
			{
				neighbers.push_back(neighbor);
				topNeighbors.push_back(neighbor);
			}
		}
	}
}

std::vector<Cell*>* Rectangle::GetCells()
{
    return cells;
}

void Rectangle::recalculateSize()
{
	if (cells->empty()) { width = height = 0; return; }
	int minX = cells->front()->X(), maxX = minX;
	int minY = cells->front()->Y(), maxY = minY;
	for (auto* cell : *cells) {
		minX = std::min(minX, cell->X()); maxX = std::max(maxX, cell->X());
		minY = std::min(minY, cell->Y()); maxY = std::max(maxY, cell->Y());
	}
	width = maxX - minX + 1;
	height = maxY - minY + 1;
}

Rectangle* Rectangle::GetNeighbor(int number) const
{
	return neighbers.at(number);
}

Rectangle* Rectangle::GetTopNeighbor(int number) const
{
	return topNeighbors.at(number);
}

Rectangle* Rectangle::GetLeftNeighbor(int number) const
{
	return leftNeighbors.at(number);
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

int Rectangle::CountTopNeighbors() const
{
	return topNeighbors.size();
}

int Rectangle::CountLeftNeighbors() const
{
	return leftNeighbors.size();
}

std::string Rectangle::GetMechanicLayer0() const
{
	return mechanicLayer0;
}

std::string Rectangle::GetMechanicLayer1() const
{
    return mechanicLayer1;
}

std::string Rectangle::GetMechanicLayer2() const
{
    return mechanicLayer2;
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
	return colorNumber == rectangle.colorNumber;
}

void Rectangle::SetMechanicLayer0(std::string MechanicName) {
	mechanicLayer0 = MechanicName;
}

void Rectangle::SetMechanicLayer1(std::string MechanicName) {
    mechanicLayer1 = MechanicName;
}

void Rectangle::SetMechanicLayer2(std::string MechanicName) {
    mechanicLayer2 = MechanicName;
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
	if (X < 0 || Y < 0 || X >= width || Y >= height || cells->empty())
		throw std::out_of_range("Cell coordinates outside rectangle.");
	int minX = cells->front()->X(), minY = cells->front()->Y();
	for (auto* cell : *cells) { minX = std::min(minX, cell->X()); minY = std::min(minY, cell->Y()); }
	for (auto* cell : *cells) if (cell->X() == minX + X && cell->Y() == minY + Y) return cell;
	throw std::out_of_range("Cell is missing from rectangle.");
}

const std::vector<Cell*>* Rectangle::getCells() const
{
	return cells;
}

Rectangle* Rectangle::Split()
{
	if (Area() != width * height)
		throw std::logic_error("Cannot split a rectangle with missing cells.");
	Random rng{};

	const bool canSplitVertical =
		width >= 2 * minWidth;

	const bool canSplitHorizontal =
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

	auto* daughterCells = new std::vector<Cell*>();

	if (splitVertical)
	{
		const int daughterWidth =
			rng.randomInt(minWidth, width - minWidth);

		// Collect right side.
		for (int y = 0; y < height; ++y)
		{
			for (int x = width - daughterWidth; x < width; ++x)
			{
				daughterCells->push_back(getCell(x, y));
			}
		}
	}
	else
	{
		const int daughterHeight =
			rng.randomInt(minHeight, height - minHeight);

		// Collect bottom side.
		for (int y = height - daughterHeight; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				daughterCells->push_back(getCell(x, y));
			}
		}
	}

	// Remove daughter's cells from this rectangle.
	for (auto* cell : *daughterCells)
	{
		auto it = std::find(
			cells->begin(),
			cells->end(),
			cell
		);

		if (it != cells->end())
		{
			cells->erase(it);
		}
	}

	sortCells();
	recalculateSize();

	auto* daughterRectangle =
		new Rectangle(daughterCells);

	return daughterRectangle;
}

Region* Rectangle::GetRegion() const
{
	return region;
}

void Rectangle::SetRegion(Region* Reg)
{
	region = Reg;
}

//bool Rectangle::TestNeighbors()
//{
//    bool correct = true;
//
//    std::vector<Rectangle*> expectedLeft;
//    std::vector<Rectangle*> expectedTop;
//
//    // ---------------------------------
//    // Calculate expected neighbors
//    // directly from cells
//    // ---------------------------------
//
//    for (auto* cell : *cells)
//    {
//        // LEFT
//        Rectangle* left = cell->LeftRectangle();
//
//        if (left != nullptr &&
//            left != this)
//        {
//            if (std::find(
//                expectedLeft.begin(),
//                expectedLeft.end(),
//                left) == expectedLeft.end())
//            {
//                expectedLeft.push_back(left);
//            }
//        }
//
//        // TOP in your naming convention
//        // actually means BottomRectangle()
//        Rectangle* top = cell->BottomRectangle();
//
//        if (top != nullptr &&
//            top != this)
//        {
//            if (std::find(
//                expectedTop.begin(),
//                expectedTop.end(),
//                top) == expectedTop.end())
//            {
//                expectedTop.push_back(top);
//            }
//        }
//    }
//
//    // ---------------------------------
//    // Print rectangle
//    // ---------------------------------
//
//    std::cout
//        << "\n================================\n";
//
//    std::cout
//        << "Rectangle: "
//        << this
//        << "\n";
//
//    if (GetRegion() != nullptr)
//    {
//        std::cout
//            << "Region: "
//            << GetRegion()
//            << "\n";
//
//        std::cout
//            << "Mechanic: "
//            << GetMechanicLayer1()
//            << "\n";
//    }
//
//    // ---------------------------------
//    // LEFT
//    // ---------------------------------
//
//    std::cout << "\nLEFT NEIGHBORS\n";
//
//    std::cout << "Expected: ";
//
//    if (expectedLeft.empty())
//        std::cout << "NONE";
//
//    for (auto* rectangle : expectedLeft)
//    {
//        std::cout << rectangle << " ";
//    }
//
//    std::cout << "\nStored:   ";
//
//    if (leftNeighbors.empty())
//        std::cout << "NONE";
//
//    for (auto* rectangle : leftNeighbors)
//    {
//        std::cout << rectangle << " ";
//    }
//
//    std::cout << "\n";
//
//    // Check missing left neighbors
//    for (auto* expected : expectedLeft)
//    {
//        if (std::find(
//            leftNeighbors.begin(),
//            leftNeighbors.end(),
//            expected) == leftNeighbors.end())
//        {
//            std::cout
//                << "ERROR: Missing LEFT neighbor: "
//                << expected
//                << "\n";
//
//            correct = false;
//        }
//    }
//
//    // Check extra left neighbors
//    for (auto* stored : leftNeighbors)
//    {
//        if (std::find(
//            expectedLeft.begin(),
//            expectedLeft.end(),
//            stored) == expectedLeft.end())
//        {
//            std::cout
//                << "ERROR: Extra LEFT neighbor: "
//                << stored
//                << "\n";
//
//            correct = false;
//        }
//    }
//
//    // ---------------------------------
//    // TOP
//    // ---------------------------------
//
//    std::cout << "\nTOP NEIGHBORS (BottomRectangle)\n";
//
//    std::cout << "Expected: ";
//
//    if (expectedTop.empty())
//        std::cout << "NONE";
//
//    for (auto* rectangle : expectedTop)
//    {
//        std::cout << rectangle << " ";
//    }
//
//    std::cout << "\nStored:   ";
//
//    if (topNeighbors.empty())
//        std::cout << "NONE";
//
//    for (auto* rectangle : topNeighbors)
//    {
//        std::cout << rectangle << " ";
//    }
//
//    std::cout << "\n";
//
//    // Check missing top neighbors
//    for (auto* expected : expectedTop)
//    {
//        if (std::find(
//            topNeighbors.begin(),
//            topNeighbors.end(),
//            expected) == topNeighbors.end())
//        {
//            std::cout
//                << "ERROR: Missing TOP neighbor: "
//                << expected
//                << "\n";
//
//            correct = false;
//        }
//    }
//
//    // Check extra top neighbors
//    for (auto* stored : topNeighbors)
//    {
//        if (std::find(
//            expectedTop.begin(),
//            expectedTop.end(),
//            stored) == expectedTop.end())
//        {
//            std::cout
//                << "ERROR: Extra TOP neighbor: "
//                << stored
//                << "\n";
//
//            correct = false;
//        }
//    }
//
//    // ---------------------------------
//    // Result
//    // ---------------------------------
//
//    if (correct)
//    {
//        std::cout << "\nRESULT: OK\n";
//    }
//    else
//    {
//        std::cout << "\nRESULT: ERROR\n";
//    }
//
//    std::cout
//        << "================================\n";
//
//    return correct;
//}
