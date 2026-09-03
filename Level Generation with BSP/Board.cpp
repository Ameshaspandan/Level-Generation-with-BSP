#include "Board.h"
#include <iomanip>
#include <random>

Board::Board(int Width, int Height) :
    height{ Height }, width{ Width }, rectangles{}, cells{}, minHeight{ 1 }, minWidth{ 1 }, rng{ std::random_device{}() }, colorRectangles{}
{
    Rectangle* baseRectangle = new Rectangle();
    rectangles.push_back(baseRectangle);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			Cell* newCell = new Cell(x, y, rectangles.at(0), x > 0 ? getCell(x - 1, y) : nullptr, y > 0 ? getCell(x, y - 1) : nullptr);
            cells.push_back(newCell);
		}
	}
}

Board::~Board()
{
    for (int n = 0; n < rectangles.size(); n++)
    {
        delete rectangles.at(n);
    }
    for (int n = 0; n < cells.size(); n++)
    {
        delete cells.at(n);
    }

    for (int i = 0; i < colorRectangles.size(); i++)
    {
        delete colorRectangles.at(i);
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

bool Board::rectangleSplit(Rectangle* motherRectangle)
{
    int motherWidth = motherRectangle->Width();
    int motherHeight = motherRectangle->Height();

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

    Rectangle* daughterRectangle = new Rectangle();

    rectangles.push_back(daughterRectangle);

    if (splitVertical)
    {
        // Vertical split
        int daughterWidth = randomInt(minWidth, motherWidth - minWidth);

        for (int x = motherWidth - daughterWidth; x < motherWidth; x++)
        {
            for (int y = 0; y < motherHeight; y++)
            {
                getCell(x + motherRectangle->BaseCell()->X(), y + motherRectangle->BaseCell()->Y())->ChangeRectangle(daughterRectangle);
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
                getCell(x + motherRectangle->BaseCell()->X(), y + motherRectangle->BaseCell()->Y())->ChangeRectangle(daughterRectangle);
            }
        }
    }
    return true;
}

void Board::mesh(int ColorCount)
{
    rectangleSort();
    for (int i = 0; i < rectangles.size(); i++)
    {
        rectangles.at(i)->FindNeigbers();
    }

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
}

void Board::findColorRectangle()
{
    // Clear previous color groups
    for (auto* colorRectangle : colorRectangles)
    {
        delete colorRectangle;
    }

    colorRectangles.clear();

    // Group rectangles by color
    for (auto* rectangle : rectangles)
    {
        bool found = false;

        for (auto* colorRectangle : colorRectangles)
        {
            if (colorRectangle->empty())
                continue;

            // All rectangles in this vector have the same color.
            if (colorRectangle->at(0)->GetColor() == rectangle->GetColor())
            {
                colorRectangle->push_back(rectangle);
                found = true;
                break;
            }
        }

        // No existing group for this color
        if (!found)
        {
            auto* colorRectangle = new std::vector<Rectangle*>();
            colorRectangle->push_back(rectangle);

            colorRectangles.push_back(colorRectangle);
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
    if (rectangles.empty())
        return;

    if (colorRectangles.empty())
        return;

    // ============================================================
    // RULE 1
    //
    // m3 requires every rectangle in the color group to have
    // even width AND even height.
    // ============================================================

    auto isValidForM3 =
        [&](std::vector<Rectangle*>* colorRectangle) -> bool
        {
            if (colorRectangle->empty())
                return false;

            for (Rectangle* rectangle : *colorRectangle)
            {
                if (rectangle->Width() % 2 != 0 ||
                    rectangle->Height() % 2 != 0)
                {
                    return false;
                }
            }

            return true;
        };

    // ============================================================
    // Helper: check whether a rectangle touches the board edge
    // ============================================================

    auto touchesBoardEdge =
        [&](Rectangle* rectangle) -> bool
        {
            int x = rectangle->BaseCell()->X();
            int y = rectangle->BaseCell()->Y();

            int right =
                x + rectangle->Width();

            int bottom =
                y + rectangle->Height();

            return
                x == 0 ||
                y == 0 ||
                right == width ||
                bottom == height;
        };

    // ============================================================
    // RULE 2
    //
    // m2/m3/m4 cannot be the first cell after m0 in any column.
    //
    // Because of Rule 6, we check the entire color group.
    // ============================================================

    auto violatesRule2 =
        [&](std::vector<Rectangle*>* colorRectangle) -> bool
        {
            for (int x = 0; x < width; x++)
            {
                bool foundM0 = false;

                for (int y = 0; y < height; y++)
                {
                    Cell* cell =
                        getCell(x, y);

                    if (cell == nullptr)
                        continue;

                    Rectangle* rectangle =
                        cell->GetRectangle();

                    if (rectangle == nullptr)
                        continue;

                    // Found m0.
                    if (rectangle->GetMechanic() == 0)
                    {
                        foundM0 = true;
                        continue;
                    }

                    // First cell after m0.
                    if (foundM0)
                    {
                        for (Rectangle* candidate : *colorRectangle)
                        {
                            if (candidate == rectangle)
                            {
                                return true;
                            }
                        }

                        break;
                    }
                }
            }

            return false;
        };

    // ============================================================
    // RULE 4
    //
    // m4 must:
    //
    // EDGE -> m4
    //
    // OR
    //
    // EDGE -> m0 -> m4
    //
    // OR
    //
    // EDGE -> m0 -> m0 -> m4
    //
    // etc.
    // ============================================================

    auto isValidForM4 =
        [&](std::vector<Rectangle*>* colorRectangle) -> bool
        {
            for (Rectangle* startRectangle : *colorRectangle)
            {
                // m4 itself touches edge.
                if (touchesBoardEdge(startRectangle))
                    return true;

                // ----------------------------------------------------
                // Search through m0 rectangles.
                // ----------------------------------------------------

                std::vector<Rectangle*> queue;
                std::vector<Rectangle*> visited;

                for (int i = 0;
                    i < startRectangle->CountNeighbors();
                    i++)
                {
                    Rectangle* neighbor =
                        startRectangle->GetNeighbor(i);

                    if (neighbor == nullptr)
                        continue;

                    if (neighbor->GetMechanic() != 0)
                        continue;

                    queue.push_back(neighbor);
                    visited.push_back(neighbor);
                }

                size_t queueIndex = 0;

                while (queueIndex < queue.size())
                {
                    Rectangle* current =
                        queue[queueIndex++];

                    // m0 reached board edge.
                    if (touchesBoardEdge(current))
                        return true;

                    for (int i = 0;
                        i < current->CountNeighbors();
                        i++)
                    {
                        Rectangle* neighbor =
                            current->GetNeighbor(i);

                        if (neighbor == nullptr)
                            continue;

                        if (neighbor->GetMechanic() != 0)
                            continue;

                        bool alreadyVisited = false;

                        for (Rectangle* visitedRectangle : visited)
                        {
                            if (visitedRectangle == neighbor)
                            {
                                alreadyVisited = true;
                                break;
                            }
                        }

                        if (alreadyVisited)
                            continue;

                        visited.push_back(neighbor);
                        queue.push_back(neighbor);
                    }
                }
            }

            return false;
        };

    // ============================================================
    // Reset mechanics
    // ============================================================

    for (auto* colorRectangle : colorRectangles)
    {
        setColorMechanic(
            colorRectangle,
            -1
        );
    }

    // ============================================================
    // RULE 3
    //
    // m0 = smallest COLOR GROUP by total area.
    // ============================================================

    auto* smallestColorRectangle =
        colorRectangles[0];

    int smallestArea = 0;

    for (Rectangle* rectangle : *smallestColorRectangle)
    {
        smallestArea += rectangle->Area();
    }

    for (auto* colorRectangle : colorRectangles)
    {
        int area = 0;

        for (Rectangle* rectangle : *colorRectangle)
        {
            area += rectangle->Area();
        }

        if (area < smallestArea)
        {
            smallestArea = area;
            smallestColorRectangle = colorRectangle;
        }
    }

    auto* m0ColorRectangle =
        smallestColorRectangle;

    setColorMechanic(
        m0ColorRectangle,
        0
    );

    // ============================================================
    // RULE 5
    //
    // RESERVE m1 BEFORE m3/m4.
    //
    // At least one m1 color group must touch the RIGHT edge.
    //
    // Because of Rule 6, the entire color group becomes m1.
    // ============================================================

    std::vector<std::vector<Rectangle*>*> m1Candidates;

    for (auto* colorRectangle : colorRectangles)
    {
        // m0 cannot also be m1.
        if (colorRectangle == m0ColorRectangle)
            continue;

        bool touchesRightEdge = false;

        for (Rectangle* rectangle : *colorRectangle)
        {
            int right =
                rectangle->BaseCell()->X() +
                rectangle->Width();

            if (right == width)
            {
                touchesRightEdge = true;
                break;
            }
        }

        if (touchesRightEdge)
        {
            m1Candidates.push_back(colorRectangle);
        }
    }

    // ------------------------------------------------------------
    // Reserve one m1 group.
    // ------------------------------------------------------------

    std::vector<Rectangle*>* reservedM1 = nullptr;

    if (!m1Candidates.empty())
    {
        int index =
            randomInt(
                0,
                static_cast<int>(m1Candidates.size()) - 1
            );

        reservedM1 =
            m1Candidates[index];

        setColorMechanic(
            reservedM1,
            1
        );
    }

    // ============================================================
    // RULE 1 + RULE 2
    //
    // Find m3 candidates.
    // ============================================================

    std::vector<std::vector<Rectangle*>*> m3Candidates;

    for (auto* colorRectangle : colorRectangles)
    {
        if (colorRectangle == m0ColorRectangle)
            continue;

        if (colorRectangle == reservedM1)
            continue;

        if (colorRectangle->at(0)->GetMechanic() != -1)
            continue;

        // Rule 1.
        if (!isValidForM3(colorRectangle))
            continue;

        // Rule 2.
        if (violatesRule2(colorRectangle))
            continue;

        m3Candidates.push_back(colorRectangle);
    }

    // ------------------------------------------------------------
    // Select m3.
    // ------------------------------------------------------------

    int m3Count =
        static_cast<int>(
            m3Candidates.size() / 4
            );

    if (!m3Candidates.empty() &&
        m3Count == 0)
    {
        m3Count = 1;
    }

    for (int i = 0; i < m3Count; i++)
    {
        int index =
            randomInt(
                i,
                static_cast<int>(
                    m3Candidates.size()
                    ) - 1
            );

        std::swap(
            m3Candidates[i],
            m3Candidates[index]
        );

        setColorMechanic(
            m3Candidates[i],
            3
        );
    }

    // ============================================================
    // RULE 4 + RULE 2
    //
    // Find m4 candidates.
    // ============================================================

    std::vector<std::vector<Rectangle*>*> m4Candidates;

    for (auto* colorRectangle : colorRectangles)
    {
        if (colorRectangle == m0ColorRectangle)
            continue;

        if (colorRectangle == reservedM1)
            continue;

        if (colorRectangle->at(0)->GetMechanic() != -1)
            continue;

        // Rule 4.
        if (!isValidForM4(colorRectangle))
            continue;

        // Rule 2 now ALSO applies to m4.
        if (violatesRule2(colorRectangle))
            continue;

        m4Candidates.push_back(colorRectangle);
    }

    // ------------------------------------------------------------
    // Select m4.
    // ------------------------------------------------------------

    int m4Count =
        static_cast<int>(
            m4Candidates.size() / 3
            );

    if (!m4Candidates.empty() &&
        m4Count == 0)
    {
        m4Count = 1;
    }

    for (int i = 0; i < m4Count; i++)
    {
        int index =
            randomInt(
                i,
                static_cast<int>(
                    m4Candidates.size()
                    ) - 1
            );

        std::swap(
            m4Candidates[i],
            m4Candidates[index]
        );

        setColorMechanic(
            m4Candidates[i],
            4
        );
    }

    // ============================================================
    // Remaining color groups -> m2 / m5
    //
    // m1 has already been reserved.
    // m3 and m4 have already been assigned.
    //
    // Rule 2 applies to m2.
    // ============================================================

    for (auto* colorRectangle : colorRectangles)
    {
        if (colorRectangle->at(0)->GetMechanic() != -1)
            continue;

        // --------------------------------------------------------
        // Try m2.
        // --------------------------------------------------------

        if (randomInt(0, 99) < 50)
        {
            if (!violatesRule2(colorRectangle))
            {
                setColorMechanic(
                    colorRectangle,
                    2
                );
            }
            else
            {
                setColorMechanic(
                    colorRectangle,
                    5
                );
            }
        }

        // --------------------------------------------------------
        // m5.
        // --------------------------------------------------------

        else
        {
            setColorMechanic(
                colorRectangle,
                5
            );
        }
    }
}

void Board::LevelGenerate(int RectangleCount, int ColorCount)
{
    resetRectangle();

    for (int n = 0; n < RectangleCount - 1; n++)
    {
        for (int i = 0; i < tryLimit; i++)
        {
            if (rectangleSplit(rectangles.at(randomInt(0, rectangles.size() - 1)))) break;
        }
    }

    mesh(ColorCount);
    paint();

    int m1Area = 0;

    for (auto* rectangle : rectangles)
    {
        if (rectangle->GetMechanic() == 1)
        {
            m1Area += rectangle->Area();
        }
    }
    if (m1Area < smallestM1Area || m1Area > largetM1Area) LevelGenerate(RectangleCount, ColorCount);
}