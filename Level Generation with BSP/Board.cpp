#include "Board.h"
#include <iomanip>
#include <random>

Board::Board(int Width, int Height) :
    height{ Height }, width{ Width }, rectangles{}, cells{}, minHeight{ 1 }, minWidth{ 1 }, rng{ std::random_device{}() }, regions{}
{
    Rectangle* baseRectangle = new Rectangle(Width, Height);
    rectangles.push_back(baseRectangle);

    cells = *baseRectangle->getCells();
}

Board::~Board()
{
    for (int n = 0; n < rectangles.size(); n++)
    {
        delete rectangles.at(n);
    }

    for (int i = 0; i < regions.size(); i++)
    {
        delete regions.at(i);
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

        //for (int x = board.width - 2; x >= 0; x--)
        //{
        //    Output
        //        << std::setw(2)
        //        << *board.getCell(x, y)
        //        << ",";
        //}
        Output << "\n";
    }
	return Output;
}

void Board::mesh(int ColorCount)
{
    rectangleSort();

    for (int i = 0; i < rectangles.size(); i++)
    {
        rectangles.at(i)->SetColor(i);
    }

    for (int n = 0; n < rectangles.size() - ColorCount; n++)
    {
        std::vector<Rectangle*> validRectangles;

        // Find rectangles that have at least one neighbor
        // with a different mechanic.
        for (Rectangle* rectangle : rectangles)
        {
            for (int i = 0; i < rectangle->CountNeighbors(); i++)
            {
                Rectangle* neighbor = rectangle->GetNeighbor(i);

                if (neighbor->GetColor() != rectangle->GetColor())
                {
                    validRectangles.push_back(rectangle);
                    break;
                }
            }
        }

        if (validRectangles.empty())
            break;

        // Choose a base rectangle.
        int baseIndex = randomInt(0, validRectangles.size() - 1);
        Rectangle* baseRectangle = validRectangles.at(baseIndex);

        // Find neighbors with a different mechanic.
        std::vector<Rectangle*> differentNeighbors;

        for (int i = 0; i < baseRectangle->CountNeighbors(); i++)
        {
            Rectangle* neighbor = baseRectangle->GetNeighbor(i);

            if (neighbor->GetColor() != baseRectangle->GetColor())
            {
                differentNeighbors.push_back(neighbor);
            }
        }

        if (differentNeighbors.empty())
            continue;

        // Choose one different-mechanic neighbor.
        int neighborIndex =
            randomInt(0, differentNeighbors.size() - 1);

        Rectangle* neighbor = differentNeighbors.at(neighborIndex);

        int baseColor = baseRectangle->GetColor();
        int neighborColor = neighbor->GetColor();

        // Merge the entire neighbor mechanic group
        // into the base mechanic group.
        for (Rectangle* rectangle : rectangles)
        {
            if (rectangle->GetColor() == neighborColor)
            {
                rectangle->SetColor(baseColor);
            }
        }
    }
    findColorRectangle();

    int m = 0;
    for (auto* rect : rectangles)
    {
        rect->SetMechanic(m++);
    }
}

void Board::findColorRectangle()
{
    // Clear previous color groups
    for (auto* region : regions)
    {
        delete region;
    }

    regions.clear();

    // Group rectangles by color
    for (auto* rectangle : rectangles)
    {
        bool found = false;

        for (auto* region : regions)
        {
            if (region->Size() == 0)
                continue;

            // All rectangles in this vector have the same color.
            if (region->GetColor() == rectangle->GetColor())
            {
                region->AddRecangle(rectangle);
                found = true;
                break;
            }
        }

        // No existing group for this color
        if (!found)
        {
            auto* newRegion = new Region();
            newRegion->AddRecangle(rectangle);

            regions.push_back(newRegion);
        }
    }
}

void Board::setColorMechanic(std::vector<Rectangle*>* ColorRectangle, int Mechanic)
{
    for (Rectangle* rectangle : *ColorRectangle)
    {
        rectangle->SetMechanic(Mechanic);
    }
}

int Board::getColorColorRectangle(std::vector<Rectangle*>* ColorRectangle)
{
    return ColorRectangle->at(0)->GetColor();
}

bool Board::touchesBoardEdge(Rectangle* Rectangle)
{
    int x = Rectangle->BaseCell()->X();
    int y = Rectangle->BaseCell()->Y();

    int right = x + Rectangle->Width();
    int bottom = y + Rectangle->Height();

    return
        x == 0 ||
        y == 0 ||
        right == width ||
        bottom == height;
}

void Board::paint()
{
   
}

void Board::LevelGenerate(int RectangleCount, int ColorCount)
{
    resetRectangle();

    for (int n = 0; n < RectangleCount - 1; n++)
    {
        for (int i = 0; i < tryLimit; i++)
        {
            Rectangle* newRecangle = rectangles.at(randomInt(0, rectangles.size() - 1))->Split();
            if (newRecangle != nullptr)
            {
                rectangles.push_back(newRecangle);
                break;
            }
        }
    }

    mesh(ColorCount);

    //paint();

    //int m1Area = 0;

    //for (auto* rectangle : rectangles)
    //{
    //    if (rectangle->GetMechanic() == 1)
    //    {
    //        m1Area += rectangle->Area();
    //    }
    //}
    //if (m1Area < smallestM1Area || m1Area > largetM1Area) LevelGenerate(RectangleCount, ColorCount);
}